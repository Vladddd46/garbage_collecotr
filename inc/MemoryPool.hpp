#pragma once
#include <vector>

// Реалізація пулу пам’яті для малих об'єктів
class MemoryPool {
    size_t chunkSize;
    size_t poolSize;
    char* pool;
    std::vector<void*> freeList;

public:
    MemoryPool(size_t chunkSize, size_t poolSize);

    ~MemoryPool();

    // Виділення блоку пам’яті з пулу
    void* allocate();

    // Повернення блоку у пул
    void deallocate(void* ptr);
};