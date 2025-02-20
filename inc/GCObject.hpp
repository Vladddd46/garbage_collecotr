#pragma once
#include <vector>

class GCObject {
public:
    bool marked = false;
    int age = 0;
    // Якщо об'єкт переміщено, тут зберігається нова адреса
    GCObject* forwarding = nullptr;

    // Обхід внутрішніх посилань (для маркування)
    virtual void traverse(std::vector<GCObject*>& children) {}

    // Повертає розмір об'єкта (необхідно для компактизації)
    virtual size_t getSize() const = 0;

    // Переміщує об'єкт за допомогою placement new у нове розташування.
    // Реалізація має записати нову адресу у поле forwarding.
    virtual void relocate(void* newLocation) = 0;

    // Оновлює внутрішні посилання, використовуючи forwarding-значення, якщо потрібно.
    virtual void updateReferences() {}

    virtual ~GCObject() {}
};
