#pragma once
#include <vector>
#include "GCObject.hpp"
#include <mutex>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <new>
#include <cassert>
#include <utility>
#include "DefaultAllocator.hpp"

// Garbage Collector з генераційним підходом
class GarbageCollector {
public:
    std::vector<GCObject*> eden;
    std::vector<GCObject*> oldGen;

    // Пул пам’яті для малих об’єктів
    MemoryPool pool;

    std::mutex gcMutex;

    GarbageCollector(size_t poolChunkSize = 64, size_t poolCapacity = 1024);

    // Шаблонний метод для алокації об'єктів через GC з використанням заданого аллокатора
    template <typename T, typename Allocator = DefaultAllocator<T>, typename... Args>
	T* allocate(Args&&... args) {
	    std::lock_guard<std::mutex> lock(gcMutex);
	    T* obj = Allocator::allocate(pool);
	    new(obj) T(std::forward<Args>(args)...);
	    eden.push_back(obj);
	    return obj;
	}


    // Фаза маркування: рекурсивно обходить об'єкти, позначаючи досяжні
    void mark(std::vector<GCObject*>& roots);

    // Фаза очищення: видаляє об'єкти, що не були позначені
    void sweep(std::vector<GCObject*>& generation);

    // Minor GC – збір сміття для молодого покоління 
    void collectMinor();

    // Major GC – збір сміття для обох генерацій
    void collectMajor();

    // Функція для отримання "кореневих" об'єктів 
    // У реальній системі корені беруться зі стеку, глобальних змінних та реєстрів.
    // Для демонстрації припустимо, що всі об'єкти із oldGen є коренями.
    std::vector<GCObject*> getRoots();
};