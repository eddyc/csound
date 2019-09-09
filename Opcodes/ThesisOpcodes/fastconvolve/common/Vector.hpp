#include <functional>
#include <iostream>

using namespace std;

#ifndef _Vector_
#define _Vector_
template <typename T>
class Vector {
private:
    const T* data = nullptr;

public:
    const size_t elementCount = 0;
    Vector(function<T*(size_t input)> memoryAllocator, size_t);
    Vector(T* data, size_t);
    Vector<T>& operator=(const Vector<T>& other);
    ~Vector();

    const Vector<T>& push(const Vector<T>&) const;
    const Vector<T>& add(const Vector<T>&) const;
    const Vector<T>& fill(T) const;
    const Vector<T>& ramp(T, T) const;
    const Vector<T>& shift(int) const;
    const Vector<T> sub(size_t length, size_t offset) const;
    static void copy(const Vector<T>& from, const Vector<T>& to);
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

#endif
