#include "GarbageCollector.hpp"
#include "GlobalMemoryPool.hpp"

GarbageCollector::GarbageCollector(size_t poolChunkSize, size_t poolCapacity)
{}

void GarbageCollector::mark(std::vector<GCObject*>& roots) {
	while (!roots.empty()) {
		GCObject* obj = roots.back();
		roots.pop_back();
		if (!obj || obj->marked)
			continue;
		obj->marked = true;
		// Отримання дітей (посилань) об'єкта
		std::vector<GCObject*> children;
		obj->traverse(children);
		for (auto child : children) {
			if (child && !child->marked) {
				roots.push_back(child);
			}
		}
	}
}

// Фаза очищення: видаляє об'єкти, що не були позначені
void GarbageCollector::sweep(std::vector<GCObject*>& generation) {
	auto it = generation.begin();
	while (it != generation.end()) {
		GCObject* obj = *it;
		if (!obj->marked) {
			obj->~GCObject();
			globalMemoryPool.deallocate(obj);
			it = generation.erase(it);
		} else {
			obj->marked = false;
			++it;
		}
	}
}

// Minor GC – збір сміття для молодого покоління 
void GarbageCollector::collectMinor() {
	std::lock_guard<std::mutex> lock(gcMutex);
	std::vector<GCObject*> roots = getRoots();
	mark(roots);
	sweep(eden);
	// Переміщення виживших об'єктів до старшої генерації
	for (GCObject* obj : eden) {
		oldGen.push_back(obj);
	}
	eden.clear();
}

// Major GC – збір сміття для обох генерацій
void GarbageCollector::collectMajor() {
	std::lock_guard<std::mutex> lock(gcMutex);
	std::vector<GCObject*> roots = getRoots();
	mark(roots);
	sweep(eden);
	sweep(oldGen);
}

// Функція для отримання "кореневих" об'єктів 
// У реальній системі корені беруться зі стеку, глобальних змінних та реєстрів.
// Для демонстрації припустимо, що всі об'єкти із oldGen є коренями.
std::vector<GCObject*> GarbageCollector::getRoots() {
	return oldGen;
}
