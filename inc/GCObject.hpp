#pragma once
#include <vector>

// Базовий клас для GC-керованих об'єктів
class GCObject {
public:
    bool marked = false;

    // Метод для обходу внутрішніх посилань
    virtual void traverse(std::vector<GCObject*>& children) {}
    virtual ~GCObject() {}
};