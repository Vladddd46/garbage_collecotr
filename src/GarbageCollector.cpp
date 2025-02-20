#include "GarbageCollector.hpp"
#include "GlobalMemoryPool.hpp"
#include <algorithm> // Для std::sort

GarbageCollector::GarbageCollector(size_t poolChunkSize, size_t poolCapacity)
{
    // Ініціалізація може бути додатково проведена, якщо потрібно.
}

// Рекурсивне маркування досяжних об'єктів
void GarbageCollector::mark(std::vector<GCObject*>& roots) {
    while (!roots.empty()) {
        GCObject* obj = roots.back();
        roots.pop_back();
        if (!obj || obj->marked)
            continue;
        obj->marked = true;
        std::vector<GCObject*> children;
        obj->traverse(children);
        for (auto child : children) {
            if (child && !child->marked) {
                roots.push_back(child);
            }
        }
    }
}

// Очищення покоління: знищення об'єктів, що не були позначені
void GarbageCollector::sweep(std::vector<GCObject*>& generation) {
    auto it = generation.begin();
    while (it != generation.end()) {
        GCObject* obj = *it;
        if (!obj->marked) {
            obj->~GCObject();
            globalMemoryPool.deallocate(obj);
            it = generation.erase(it);
        } else {
            obj->marked = false; // Скидання мітки для наступного циклу
            ++it;
        }
    }
}

// Minor GC: обробка Eden та Survivor Spaces
void GarbageCollector::collectMinor() {
    std::lock_guard<std::mutex> lock(gcMutex);
    
    // Вважаємо OldGen і поточний survivor0 як корені
    std::vector<GCObject*> roots = getRoots();
    mark(roots);

    const int promotionThreshold = 1; // Наприклад, якщо obj->age >= threshold, просуваємо до OldGen

    std::vector<GCObject*> newSurvivors;

    // Обробка Eden: виживші об'єкти переміщуються до нового survivor або просуваються до OldGen
    for (GCObject* obj : eden) {
        if (obj->marked) {
            obj->age++;
            if (obj->age < promotionThreshold) {
                newSurvivors.push_back(obj);
            } else {
                oldGen.push_back(obj);
            }
        } else {
            obj->~GCObject();
            globalMemoryPool.deallocate(obj);
        }
    }
    eden.clear();

    // Обробка survivor0: аналогічно
    std::vector<GCObject*> survivorsFromS0;
    for (GCObject* obj : survivor0) {
        if (obj->marked) {
            obj->age++;
            if (obj->age < promotionThreshold) {
                survivorsFromS0.push_back(obj);
            } else {
                oldGen.push_back(obj);
            }
        } else {
            obj->~GCObject();
            globalMemoryPool.deallocate(obj);
        }
    }
    // Нова survivor0 – об'єднання виживших з Eden та попереднього survivor0
    survivor0 = newSurvivors;
    survivor0.insert(survivor0.end(), survivorsFromS0.begin(), survivorsFromS0.end());

    // Очистимо survivor1 (якщо використовується)
    survivor1.clear();
}

// Major GC: повна очистка всіх поколінь із фазою компактизації OldGen
void GarbageCollector::collectMajor() {
    std::lock_guard<std::mutex> lock(gcMutex);
    
    std::vector<GCObject*> roots = getRoots();
    mark(roots);
    
    sweep(eden);
    sweep(survivor0);
    sweep(survivor1);
    sweep(oldGen);

    // Фаза компактизації для Old Generation
    compactOldGen();
}

// Для демонстрації, корені беруться з OldGen
std::vector<GCObject*> GarbageCollector::getRoots() {
    return oldGen;
}

// Фаза компактизації для Old Generation (повна реалізація)
// 1. Обчислюється сумарний розмір живих об'єктів у OldGen.
// 2. Виділяється нова безперервна область пам’яті.
// 3. Кожен живий об'єкт переміщується у нове місце за допомогою relocate().
// 4. Всі об'єкти оновлюють свої внутрішні посилання через updateReferences().
void GarbageCollector::compactOldGen() {
    // Обчислюємо сумарний розмір живих об'єктів
    size_t totalSize = 0;
    for (GCObject* obj : oldGen) {
        if (obj->marked) { // якщо живий
            totalSize += obj->getSize();
        }
    }
    if (totalSize == 0) return; // нічого не переміщувати

    // Виділяємо нову безперервну область пам’яті
    char* newRegion = static_cast<char*>(std::malloc(totalSize));
    if (!newRegion) throw std::bad_alloc();

    size_t offset = 0;
    // Переміщуємо кожен живий об'єкт OldGen у нове розташування
    for (size_t i = 0; i < oldGen.size(); i++) {
        GCObject* obj = oldGen[i];
        if (obj->marked) {
            void* newLocation = newRegion + offset;
            obj->relocate(newLocation);
            offset += obj->getSize();
            // Оновлюємо запис у OldGen – тепер він вказує на нове місце
            oldGen[i] = obj->forwarding;
        }
    }

    // Оновлюємо внутрішні посилання для всіх поколінь
    auto updateRefsInGeneration = [](std::vector<GCObject*>& gen) {
        for (GCObject* obj : gen) {
            obj->updateReferences();
        }
    };
    updateRefsInGeneration(eden);
    updateRefsInGeneration(survivor0);
    updateRefsInGeneration(survivor1);
    updateRefsInGeneration(oldGen);

    // Для демонстрації: виводимо інформацію про компактизацію
    std::cout << "Compaction complete. OldGen compacted into contiguous region of size " << totalSize << " bytes.\n";

    // За бажанням, можна зберегти newRegion як нову основу для OldGen,
    // або інтегрувати її у власний механізм керування пам’яттю для Old Generation.
    // (В даному прикладі ми не звільняємо старі сегменти з MemoryPool, оскільки вони керуються окремо.)
}
