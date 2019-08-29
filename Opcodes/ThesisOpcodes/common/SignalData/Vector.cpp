//
//  Vector.cpp
//  Signals
//
//  Created by Edward Costello on 14/01/2018.
//  Copyright © 2018 Edward Costello. All rights reserved.
//

#include "Vector.hpp"
#include <assert.h>
#include <iostream>
using namespace std;
using namespace Signals;

template <typename T>
Vector<T>::Vector(const Vector<T>& other)
    : data(other.data), elementCount(other.elementCount)
{
}

template <typename T>
Vector<T>::Vector() : data(nullptr), elementCount(0)
{
}

template <typename T>
Vector<T>::Vector(tuple<T*, size_t> t)
    : data(get<0>(t)), elementCount(get<1>(t))
{
}

template <typename T>
Vector<T>::Vector(T* data, size_t elementCount)
    : data(data), elementCount(elementCount)
{
}

template <typename T>
Vector<T>::Vector(pair<T*, size_t> p) : data(p.first), elementCount(p.second)
{
}

template <typename T>
Vector<T>::Vector(initializer_list<T> data)
    : data(data.begin()), elementCount(data.size())
{
}

template <typename T>
void Vector<T>::copy(const Vector<T> source, Vector<T> destination)
{
    assert(source.elementCount == destination.elementCount);
    memcpy((void*)destination.data, (const void*)source.data,
           source.elementCount * sizeof(T));
}

template <typename T>
void Vector<T>::copy(Vector<T> source, StridedVector<T>& destination)
{
    assert(source.elementCount == destination.elementCount);
    for (size_t i = 0; i < source.elementCount; ++i) {
        destination[i] = source[i];
    }
}

template <typename T>
void Vector<T>::toFloat(Vector<double> source, Vector<float>& destination)
{
    assert(source.elementCount == destination.elementCount);
    for (size_t i = 0; i < source.elementCount; ++i) {
        destination[i] = source[i];
    }
}

template <typename T>
void Vector<T>::toDouble(Vector<float> source, Vector<double>& destination)
{
    assert(source.elementCount == destination.elementCount);
    for (size_t i = 0; i < source.elementCount; ++i) {
        destination[i] = source[i];
    }
}

template <typename T>
Vector<T> Vector<T>::subVector(size_t offset, size_t elementCount)
{
    assert(elementCount + offset <= this->elementCount);

    return Vector<T>((T*)&data[offset], elementCount);
}

template <typename T>
Vector<T>& Vector<T>::map(function<T(T element)> func)
{
    T* data = (T*)this->data;
    for (size_t i = 0; i < this->elementCount; ++i) {
        data[i] = func(data[i]);
    }
    return *this;
}

template <typename T>
Vector<T>& Vector<T>::map(function<T(T element, size_t index)> func)
{
    T* data = (T*)this->data;
    for (size_t i = 0; i < this->elementCount; ++i) {
        data[i] = func(data[i], i);
    }
    return *this;
}

template <typename T>
T Vector<T>::reduce(function<T(T, T)> func)
{
    T* data = (T*)this->data;
    T acc = 0;
    for (size_t i = 0; i < this->elementCount; ++i) {
        acc = func(acc, data[i]);
    }
    return acc;
}

template <typename T>
T Vector<T>::reduce(function<T(T, T, size_t)> func)
{
    T* data = (T*)this->data;
    T acc = 0;
    for (size_t i = 0; i < this->elementCount; ++i) {
        acc = func(acc, data[i], i);
    }
    return acc;
}

template <typename T>
void Vector<T>::print()
{
    for (size_t i = 0; i < elementCount; ++i) {
        printf("[%f]", (*this)[i]);
    }
    cout << endl;
}

template <typename T>
T& Vector<T>::operator[](size_t index)
{
    assert(index < elementCount);
    T* data = (T*)this->data;
    return data[index];
}

template <typename T>
Vector<T>& Vector<T>::operator=(Vector<T>&& other)
{
    if (this != &other) {
        memcpy((void*)&data, &other.data, sizeof(T*));
        memcpy((void*)&elementCount, (void*)&other.elementCount, sizeof(size_t));
    }
    return *this;
}

template <typename T>
T& Vector<T>::last()
{
    return *(T*)&this->data[elementCount - 1];
}

template <typename T>
T* Vector<T>::begin()
{
    return (T*)this->data;
}

template <typename T>
T* Vector<T>::end()
{
    return (T*)((char*)this->data + elementCount * sizeof(T));
}

template <typename T>
const T* Vector<T>::cbegin() const
{
    return (const T*)this->data;
}

template <typename T>
const T* Vector<T>::cend() const
{
    return (const T*)((char*)this->data + elementCount * sizeof(T));
}

template <typename T>
const T* Vector<T>::begin() const
{
    return (const T*)this->data;
}

template <typename T>
const T* Vector<T>::end() const
{
    return (const T*)((char*)this->data + elementCount * sizeof(T));
}

template <typename T>
bool Vector<T>::equals(Vector<T>& inputA, Vector<T>& inputB)
{
    if (inputA.elementCount != inputB.elementCount)
        return false;

    for (size_t i = 0; i < inputA.elementCount; ++i) {
        if (inputA[i] != inputB[i])
            return false;
    }

    return true;
}

template <typename T>
Vector<T>& Vector<T>::shift(int offset)
{
    if (offset == 0)
        return *this;

    assert(abs(offset) < elementCount);
    size_t count = elementCount + offset;

    if (offset > 0) {
        memcpy((void*)&data[offset], (const void*)data, count * sizeof(T));
        memset((void*)data, 0, sizeof(T) * offset);
    }
    else {
        memcpy((void*)data, (const void*)&data[abs(offset)], count * sizeof(T));
        memset((void*)&data[count], 0, sizeof(T) * abs(offset));
    }

    return *this;
}

template <typename T>
StridedVector<T>::StridedVector(T* data, size_t capacity, size_t elementCount, size_t stride)
    : data(data), capacity(capacity), elementCount(elementCount), stride(stride)
{
    assert(((elementCount - 1) * stride) + 1 <= capacity);
}

template <typename T>
StridedVector<T> StridedVector<T>::subVector(size_t elementCount, size_t offset)
{
    assert(elementCount + offset <= this->elementCount);

    return StridedVector<T>((T*)&data[offset * stride],
                            capacity - offset * stride, elementCount, stride);
}

template <typename T>
T& StridedVector<T>::operator[](size_t index)
{
    assert(index < elementCount);
    T* data = (T*)this->data;
    return data[index * stride];
}

template <typename T>
T& StridedVector<T>::operator[](size_t index) const
{
    assert(index < elementCount);
    T* data = (T*)this->data;
    return data[index * stride];
}

template <typename T>
void StridedVector<T>::print()
{
    for (size_t i = 0; i < elementCount; ++i) {
        cout << "[" << (*this)[i] << "]";
    }
    cout << endl;
}

template <typename T>
void StridedVector<T>::printAll()
{
    for (size_t i = 0; i < capacity; ++i) {
        cout << "[" << this->data[i] << "]";
    }
    cout << endl;
}

template <typename T>
void StridedVector<T>::copy(const StridedVector<T> source, Vector<T> destination)
{
    assert(source.elementCount == destination.elementCount);

    for (size_t i = 0; i < source.elementCount; ++i) {
        destination[i] = source[i];
    }
}

template <typename T>
ComplexVector<T>::ComplexVector(T* real, T* imag, size_t elementCount)
    : real(Vector<T>(real, elementCount)),
      imag(Vector<T>(imag, elementCount)),
      elementCount(elementCount)
{
}

template class Signals::Vector<double>;
template class Signals::Vector<float>;
template class Signals::StridedVector<double>;
template class Signals::StridedVector<float>;
