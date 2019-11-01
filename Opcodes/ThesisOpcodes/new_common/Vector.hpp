#include "Plot.hpp"
#include "Socket.hpp"
#include <functional>
#include <iostream>
using namespace std;

#ifndef _Vector_
#define _Vector_
template <typename T>
class Vector
{
private:
public:
    T* data = nullptr;
    const size_t elementCount = 0;
    Vector(function<T*(size_t input)> memoryAllocator, size_t);
    Vector(T* data, size_t);
    Vector<T>& operator=(const Vector<T>& other);
    Vector();
    ~Vector();

    const Vector<T>& clear() const;
    const T sum() const;
    const T sum(const size_t offset, const size_t count) const;
    const Vector<T>& push(const Vector<T>&) const;
    const Vector<T>& add(const Vector<T>&) const;
    const Vector<T>& subtract(const Vector<T>&) const;
    const Vector<T>& add(const T) const;
    const Vector<T>& divide(const T) const;
    const Vector<T>& power(const Vector<T>&) const;
    static void power(const Vector<T>& a, const Vector<T>& b, const Vector<T>& output);
    const Vector<T>& fill(const T) const;
    const Vector<T>& multiply(const T) const;
    const Vector<T>& ramp(T start, T end) const;
    const Vector<T>& shift(int) const;
    const Vector<T>& reverse() const;
    const Vector<T>& hanningWindow() const;
    const Vector<T>& sineWindow() const;
    const Vector<T>& log10() const;
    const Vector<T> sub(size_t length, size_t offset) const;
    static void copy(const Vector<T>& from, const Vector<T>& to);
    static void copy(const Vector<T>& from, const Vector<T>& to, const size_t stride);
    static void multiply(const Vector<T>& a, const Vector<T>& b, const Vector<T>& output);
    static void divide(const Vector<T>& a, const Vector<T>& b, const Vector<T>& output);
    static void add(const Vector<T>& a, const Vector<T>& b, const Vector<T>& output);
    static void subtract(const Vector<T>& a, const Vector<T>& b, const Vector<T>& output);
    static void magnitude(const Vector<T>& real, const Vector<T>& imag, const Vector<T>& magnitude);
    static void magnitudeSquared(const Vector<T>& real,
                                 const Vector<T>& imag,
                                 const Vector<T>& magnitudeSquared);
    static void angle(const Vector<T>& real, const Vector<T>& imag, const Vector<T>& angle);
    static void phase(const Vector<T>& real, const Vector<T>& imag, const Vector<T>& phase);
    static void interpolate(const Vector<T>& input,
                            const Vector<T>& indices,
                            const Vector<T>& output);
    T& operator[](const size_t index) const;
    void send(const string label) const;
    void send(const char* label) const;

    void print() const;
};

template <typename T>
class ResizableVector : public Vector<T>
{
    const size_t allocatedElementCount;

public:
    ResizableVector();
    ~ResizableVector();
    ResizableVector(function<T*(size_t input)> allocator, const size_t);

    void setElementCount(const size_t) const;
};

#define graph(input) input.send(#input)

template <typename T>
class Matrix : public Vector<T>
{
public:
    const size_t rowCount, columnCount;
    Matrix(function<T*(size_t input)> memoryAllocator, const size_t, const size_t);
    Matrix(T* data, const size_t, const size_t);
    Matrix();
    ~Matrix();
    Vector<T> operator[](const size_t index) const;
    static void multiply(const Matrix<T>& a, const Matrix<T>& b, const Matrix<T>& output);
};

template <typename T>
class VectorFactory
{
    function<T*(size_t input)> memoryAllocator;

public:
    VectorFactory(function<T*(size_t input)> memoryAllocator);
    Vector<T> operator()(size_t n) const;
    Vector<T> operator()(T* data, size_t n) const;
};

#endif
