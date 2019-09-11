#include <functional>
#include <iostream>

using namespace std;

#ifndef _Vector_
#define _Vector_
template <typename T>
class Vector {
private:
public:
    const T* data = nullptr;
    const size_t elementCount = 0;
    Vector(function<T*(size_t input)> memoryAllocator, size_t);
    Vector(T* data, size_t);
    Vector<T>& operator=(const Vector<T>& other);
    ~Vector();

    const Vector<T>& push(const Vector<T>&) const;
    const Vector<T>& add(const Vector<T>&) const;
    const Vector<T>& fill(T) const;
    const Vector<T>& multiply(const T) const;
    const Vector<T>& ramp(T, T) const;
    const Vector<T>& shift(int) const;
    const Vector<T>& reverse() const;
    const Vector<T>& hanningWindow() const;
    const Vector<T> sub(size_t length, size_t offset) const;
    static void copy(const Vector<T>& from, const Vector<T>& to);
    static void multiply(const Vector<T>& a, const Vector<T>& b, const Vector<T>& output);
    static void add(const Vector<T>& a, const Vector<T>& b, const Vector<T>& output);
    T operator[](const size_t index) const;
    void print() const;
};

template <typename T>
class Matrix : public Vector<T> {
public:
    const size_t rowCount, columnCount;
    Matrix(function<T*(size_t input)> memoryAllocator, const size_t, const size_t);
    Matrix(T* data, const size_t, const size_t);

    ~Matrix();
    Vector<T> operator[](const size_t index) const;
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
