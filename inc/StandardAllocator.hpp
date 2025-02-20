#pragma once
#include "MemoryPool.hpp"
#include <new>

// Аллокатор, який використовує глобальний оператор new
template <typename T>
struct StandardAllocator {
    static T* allocate(MemoryPool& /*pool*/) {
        // Виділення пам'яті стандартним способом; пул не використовується
        void* ptr = ::operator new(sizeof(T));
        return static_cast<T*>(ptr);
    }
};
