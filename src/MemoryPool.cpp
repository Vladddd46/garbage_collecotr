#include "MemoryPool.hpp"
#include <cstdlib>   // Для std::aligned_alloc та std::free
#include <stdexcept> // Для std::bad_alloc

MemoryPool::MemoryPool(size_t chunkSize, size_t poolSize, size_t alignment)
    : chunkSize(chunkSize), poolSize(poolSize), alignment(alignment)
{
    // Обчислення загального розміру пам’яті
    size_t totalSize = chunkSize * poolSize;
    // Забезпечуємо, щоб totalSize було кратним alignment
    if (totalSize % alignment != 0) {
        totalSize = ((totalSize + alignment - 1) / alignment) * alignment;
    }

    // Виділення вирівняної пам’яті
    pool = static_cast<char*>(std::aligned_alloc(alignment, totalSize));
    if (!pool) {
        throw std::bad_alloc();
    }

    // Заповнення впорядкованого freeList: блоки будуть зростати за адресою
    for (size_t i = 0; i < poolSize; i++) {
        freeList.insert(pool + i * chunkSize);
    }
}

MemoryPool::~MemoryPool() {
    std::free(pool);
}

void* MemoryPool::allocate() {
    if (freeList.empty()) {
        throw std::bad_alloc();
    }
    // Отримуємо блок з найменшою адресою
    auto it = freeList.begin();
    void* ptr = *it;
    freeList.erase(it);
    return ptr;
}

void MemoryPool::deallocate(void* ptr) {
    // Автоматично впорядковуємо за допомогою std::set
    freeList.insert(ptr);
}
