#include "MemoryPool.hpp"

MemoryPool::MemoryPool(size_t chunkSize, size_t poolSize)
    : chunkSize(chunkSize), poolSize(poolSize)
{
    pool = new char[chunkSize * poolSize];
    for (size_t i = 0; i < poolSize; i++) {
        freeList.push_back(pool + i * chunkSize);
    }
}

MemoryPool::~MemoryPool() {
    delete[] pool;
}

void* MemoryPool::allocate() {
    if (freeList.empty()) {
        // варто реалізувати розширення пулу
        throw std::bad_alloc();
    }
    void* ptr = freeList.back();
    freeList.pop_back();
    return ptr;
}

void MemoryPool::deallocate(void* ptr) {
    freeList.push_back(ptr);
}
