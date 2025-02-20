#pragma once
#include <set>
#include <cstddef>

// Реалізація пулу пам’яті для малих об'єктів з покращеною кеш-локальністю
class MemoryPool {
    size_t chunkSize;
    size_t poolSize;
    char* pool;
    size_t alignment;
    std::set<void*> freeList; // Використання std::set для збереження блоків у впорядкованому порядку

public:
    MemoryPool(size_t chunkSize, size_t poolSize, size_t alignment = 8);
    ~MemoryPool();

    // Виділення блоку пам’яті з пулу
    void* allocate();

    // Повернення блоку у пул
    void deallocate(void* ptr);
};
