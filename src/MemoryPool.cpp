#include "MemoryPool.hpp"
#include <cstdlib>   // Для std::aligned_alloc та std::free
#include <stdexcept>

// Конструктор: перевірка розміру та початкове виділення першого сегмента
MemoryPool::MemoryPool(size_t chunkSize, size_t poolSize, size_t alignment)
    : chunkSize(chunkSize), poolSize(poolSize), alignment(alignment)
{
    if (chunkSize > 128) {
        throw std::invalid_argument("This pool is designed for small objects (<= 128 bytes).");
    }
    addSegment(); // Виділення першого сегмента
}

// Метод addSegment виділяє новий сегмент пам’яті та додає усі його блоки до freeList
void MemoryPool::addSegment() {
    size_t segmentSize = chunkSize * poolSize;
    // Забезпечуємо, щоб segmentSize було кратним alignment
    if (segmentSize % alignment != 0) {
        segmentSize = ((segmentSize + alignment - 1) / alignment) * alignment;
    }
    char* segment = static_cast<char*>(std::aligned_alloc(alignment, segmentSize));
    if (!segment) {
        throw std::bad_alloc();
    }
    segments.push_back(segment);
    // Додаємо кожен блок цього сегмента у freeList
    for (size_t i = 0; i < poolSize; i++) {
        freeList.push_back(segment + i * chunkSize);
    }
}

// Метод allocate повертає блок із freeList; якщо freeList порожня, додається новий сегмент
void* MemoryPool::allocate() {
    if (freeList.empty()) {
        addSegment();
    }
    void* ptr = freeList.back();
    freeList.pop_back();
    return ptr;
}

// Метод deallocate повертає блок у freeList для подальшого повторного використання
void MemoryPool::deallocate(void* ptr) {
    // Можна додати перевірку належності ptr одному з сегментів (опціонально)
    freeList.push_back(ptr);
}

// Деструктор звільняє усі сегменти
MemoryPool::~MemoryPool() {
    for (char* segment : segments) {
        std::free(segment);
    }
}
