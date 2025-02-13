#include "GCObject.hpp"
#include <vector>

// Приклад класу, керованого GC
class TestObject_1 : public GCObject {
public:
    int value;
    GCObject* child; // для демонстрації: посилання на інший GCObject

    TestObject_1(int v, GCObject* child = nullptr);

    // Реалізація обходу посилань для GC
    virtual void traverse(std::vector<GCObject*>& children) override;

    virtual ~TestObject_1();
};