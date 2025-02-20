#pragma once
#include "GCObject.hpp"
#include <iostream>

class TestObject_1 : public GCObject {
public:
    int value;
    GCObject* child; // для демонстрації: посилання на інший GCObject

    TestObject_1(int v, GCObject* child = nullptr) : value(v), child(child) {}

    // Обхід внутрішніх посилань
    virtual void traverse(std::vector<GCObject*>& children) override {
        if(child) children.push_back(child);
    }

    // Повертає розмір об'єкта
    virtual size_t getSize() const override {
        return sizeof(TestObject_1);
    }

    // Переміщує об'єкт у нове місце і записує нову адресу у forwarding
    virtual void relocate(void* newLocation) override {
        // Створюємо копію об'єкта у новому розташуванні через placement new
        TestObject_1* newObj = new(newLocation) TestObject_1(*this);
        // Записуємо нову адресу у forwarding
        forwarding = newObj;
    }

    // Оновлюємо вказівник на дочірній об'єкт, якщо той був переміщений
    virtual void updateReferences() override {
        if(child && child->forwarding != nullptr) {
            child = child->forwarding;
        }
    }

    virtual ~TestObject_1() {
        std::cout << "TestObject_1 with value " << value << " destroyed.\n";
    }
};
