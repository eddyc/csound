#include <functional>
#include <iostream>

using namespace std;

template <typename T>
class Vector {
private:
    const T* data = nullptr;
    const size_t elementCount = 0;

public:
    Vector(function<T*(size_t input)> memoryAllocator, size_t);
    Vector(T* data, size_t);
    Vector<T>& operator=(const Vector<T>& other);
    ~Vector();

    const Vector<T>& push(const Vector<T>&) const;
    const Vector<T>& fill(T);
    void print() const;
};

template <typename T>
class VectorFactory {
    function<T*(size_t input)> memoryAllocator;

public:
    VectorFactory(function<T*(size_t input)> memoryAllocator);
    Vector<T> operator()(size_t n) const;
    Vector<T> operator()(T* data, size_t n) const;
};
