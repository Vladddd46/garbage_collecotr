#pragma once
#include "MemoryPool.hpp"
#include "GlobalMemoryPool.hpp"
#include <new>

// Аллокатор, який використовує пул пам’яті для виділення пам’яті
template <typename T>
struct PoolAllocator {
    static T* allocate() {
        // Отримання блоку пам’яті з пулу
        return static_cast<T*>(globalMemoryPool.allocate());
    }
};


// Аллокатор, який використовує глобальний оператор new
template <typename T>
struct StandardAllocator {
    static T* allocate() {
        // Виділення пам'яті стандартним способом; пул не використовується
        void* ptr = ::operator new(sizeof(T));
        return static_cast<T*>(ptr);
    }
};
