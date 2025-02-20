#pragma once
#include <vector>
#include <cstddef>

// Пул пам’яті для малих об’єктів (до 128 байтів) з підтримкою сегментації
class MemoryPool {
    size_t chunkSize;    // Розмір одного блоку (має бути <= 128 байтів)
    size_t poolSize;     // Кількість блоків у кожному сегменті
    size_t alignment;    // Вирівнювання пам’яті
    std::vector<char*> segments;      // Список усіх сегментів, які було виділено
    std::vector<void*> freeList;        // Список вільних блоків, отриманих з усіх сегментів

public:
    // Конструктор приймає розмір блоку, кількість блоків у сегменті та параметр вирівнювання
    MemoryPool(size_t chunkSize, size_t poolSize, size_t alignment = 8);

    ~MemoryPool();

    // Виділення блоку пам’яті з пулу (якщо вільних блоків немає, викликається додаткове виділення сегмента)
    void* allocate();

    // Повернення блоку у пул для повторного використання
    void deallocate(void* ptr);

private:
    // Метод для виділення нового сегмента пам’яті та додавання його блоків до freeList
    void addSegment();
};
