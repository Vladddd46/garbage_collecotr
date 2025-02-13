#pragma once
#include "MemoryPool.hpp"

// Політика алокації (Policy-Based Design)
// Стандартна політика: використовує пул пам’яті для виділення пам’яті
template <typename T>
struct DefaultAllocator {
    static T* allocate(MemoryPool& pool) {
        // Отримуємо блок пам’яті з пулу та повертаємо вказівник типу T
        return static_cast<T*>(pool.allocate());
    }
};