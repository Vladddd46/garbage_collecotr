#pragma once
#include <vector>
#include "GCObject.hpp"
#include <mutex>
#include <iostream>
#include <cstdlib>
#include <new>
#include <cassert>
#include <utility>
#include "Allocators.hpp"

class GarbageCollector {
public:
    // Покоління:
    std::vector<GCObject*> eden;
    std::vector<GCObject*> survivor0; // Survivor Space S0
    std::vector<GCObject*> survivor1; // Survivor Space S1
    std::vector<GCObject*> oldGen;    // Old Generation

    std::mutex gcMutex;

    GarbageCollector(size_t poolChunkSize = 64, size_t poolCapacity = 1024);

    // Шаблонний метод для алокації об'єктів через GC із вибором алокатора
    template <typename T, typename Allocator = PoolAllocator<T>, typename... Args>
    T* allocate(Args&&... args) {
        std::lock_guard<std::mutex> lock(gcMutex);
        T* obj = Allocator::allocate();
        new(obj) T(std::forward<Args>(args)...);
        eden.push_back(obj);
        return obj;
    }

    // Фаза маркування (mark)
    void mark(std::vector<GCObject*>& roots);

    // Фаза очищення (sweep)
    void sweep(std::vector<GCObject*>& generation);

    // Minor GC – збір сміття для молодих об'єктів із переміщенням у Survivor Spaces
    void collectMinor();

    // Major GC – повна очистка з компактизацією Old Generation
    void collectMajor();

    // Функція для отримання "кореневих" об'єктів (для демонстрації – беремо OldGen)
    std::vector<GCObject*> getRoots();

    // Фаза компактизації для Old Generation (повна реалізація)
    void compactOldGen();
};
