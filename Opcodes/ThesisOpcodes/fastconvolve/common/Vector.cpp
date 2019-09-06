#include "Vector.hpp"
#include <Accelerate/Accelerate.h>

using namespace std;
template <typename T>
Vector<T>::Vector(function<T*(size_t input)> memoryAllocator, size_t elementCount)
    : data(memoryAllocator(elementCount)), elementCount(elementCount)
{
}

template <typename T>
Vector<T>::Vector(T* data, size_t elementCount)
    : data(data), elementCount(elementCount)
{
}

template <typename T>
Vector<T>::~Vector()
{
}

template <typename T>
Vector<T>& Vector<T>::operator=(const Vector<T>& other)
{
    if (&other == this) {
        return *this;
    }

    memcpy((void*)(this->data), (const void*)other.data, sizeof(T*));
    memcpy((void*)(this->elementCount), (const void*)other.elementCount, sizeof(size_t));
    return *this;
}

template <typename T>
VectorFactory<T>::VectorFactory(function<T*(size_t input)> memoryAllocator)
    : memoryAllocator(memoryAllocator)
{
}

template <typename T>
Vector<T> VectorFactory<T>::operator()(size_t elementCount) const
{
    return Vector<T>(memoryAllocator, elementCount);
}

template <typename T>
Vector<T> VectorFactory<T>::operator()(T* data, size_t elementCount) const
{
    Vector<T> newVec(data, elementCount);
    return newVec;
}

template <typename T>
const Vector<T>& Vector<T>::push(const Vector<T>& input) const
{
    const size_t length =
        input.elementCount >= elementCount ? elementCount : input.elementCount;

    if (length == elementCount) {
        memcpy((void*)data, (const void*)input.data, sizeof(T) * elementCount);
    }
    else {
        const size_t remainder = elementCount - length;
        const size_t remainderIndex = elementCount - remainder;
        memcpy((void*)&data[remainderIndex], (const void*)data, sizeof(T) * remainder);
        memcpy((void*)data, (const void*)input.data, sizeof(T) * input.elementCount);
    }
    return *this;
}

template <>
const Vector<double>& Vector<double>::fill(double value)
{
    vDSP_vfillD(&value, (double*)data, 1, elementCount);
    return *this;
}

template <>
const Vector<float>& Vector<float>::fill(float value)
{
    vDSP_vfill(&value, (float*)data, 1, elementCount);
    return *this;
}

template <typename T>
void Vector<T>::print() const
{
    for (size_t i = 0; i < elementCount; ++i) {
        cout << data[i] << endl;
    }
}

template class Vector<double>;
template class Vector<float>;
template class VectorFactory<double>;
template class VectorFactory<float>;
