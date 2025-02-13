#include "GCObject.hpp"
#include "MemoryPool.hpp"
#include "TestObject_1.hpp"
#include "GarbageCollector.hpp"

int main() {
    GarbageCollector gc;

    TestObject_1* obj1 = gc.allocate<TestObject_1>(10);
    TestObject_1* obj2 = gc.allocate<TestObject_1>(20);
    TestObject_1* obj3 = gc.allocate<TestObject_1>(30, obj1); // obj3 посилається на obj1

    // Для демонстрації робимо obj2 "кореневим" об'єктом
    gc.oldGen.push_back(obj2);

    std::cout << "Виконується minor GC...\n";
    gc.collectMinor();

    std::cout << "Виконується major GC...\n";
    gc.collectMajor();

    return 0;
}
