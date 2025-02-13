#include "TestObject_1.hpp"
#include <iostream>

TestObject_1::TestObject_1(int v, GCObject* child) : value(v), child(child) {}

// Реалізація обходу посилань для GC
void TestObject_1::traverse(std::vector<GCObject*>& children) {
    if (child) {
        children.push_back(child);
    }
}

TestObject_1::~TestObject_1() {
    std::cout << "TestObject_1 з value=" << value << " знищується.\n";
}