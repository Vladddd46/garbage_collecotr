#pragma once
#include "MemoryPool.hpp"

// Аллокатор, який використовує пул пам’яті для виділення пам’яті
template <typename T>
struct PoolAllocator {
    static T* allocate(MemoryPool& pool) {
        // Отримання блоку пам’яті з пулу
        return static_cast<T*>(pool.allocate());
    }
};
