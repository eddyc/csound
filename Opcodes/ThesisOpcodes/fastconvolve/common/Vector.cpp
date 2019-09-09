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
        memcpy((void*)data, (void*)&data[length], sizeof(T) * remainder);
        memcpy((void*)&data[remainder], (const void*)input.data,
               sizeof(T) * input.elementCount);
    }
    return *this;
}

template <>
const Vector<double>& Vector<double>::fill(double value) const
{
    vDSP_vfillD(&value, (double*)data, 1, elementCount);
    return *this;
}

template <>
const Vector<float>& Vector<float>::fill(float value) const
{
    vDSP_vfill(&value, (float*)data, 1, elementCount);
    return *this;
}

template <>
const Vector<double>& Vector<double>::multiply(const double value) const
{
    vDSP_vsmulD(data, 1, &value, (double*)data, 1, elementCount);
    return *this;
}

template <>
const Vector<float>& Vector<float>::multiply(const float value) const
{
    vDSP_vsmul(data, 1, &value, (float*)data, 1, elementCount);
    return *this;
}

template <>
void Vector<double>::multiply(const Vector<double>& a,
                              const Vector<double>& b,
                              const Vector<double>& output)
{
    if (a.elementCount != b.elementCount || a.elementCount != output.elementCount) {
        cout << "Vector multiply: vectors are not the same size" << endl;
        exit(-1);
    }

    vDSP_vmulD(a.data, 1, b.data, 1, (double*)output.data, 1, a.elementCount);
}

template <>
void Vector<float>::multiply(const Vector<float>& a,
                             const Vector<float>& b,
                             const Vector<float>& output)
{
    if (a.elementCount != b.elementCount || a.elementCount != output.elementCount) {
        cout << "Vector multiply: vectors are not the same size" << endl;
        exit(-1);
    }

    vDSP_vmul(a.data, 1, b.data, 1, (float*)output.data, 1, a.elementCount);
}

template <>
const Vector<float>& Vector<float>::ramp(float start, float end) const
{
    float increment = (end - start) / (float)(elementCount - 1);
    vDSP_vramp(&start, &increment, (float*)data, 1, elementCount);
    return *this;
}

template <>
const Vector<double>& Vector<double>::ramp(double start, double end) const
{
    double increment = (end - start) / (double)(elementCount - 1);
    vDSP_vrampD(&start, &increment, (double*)data, 1, elementCount);
    return *this;
}

template <>
const Vector<float>& Vector<float>::add(const Vector<float>& input) const
{
    vDSP_vadd((const float*)input.data, 1, (const float*)data, 1, (float*)data, 1, elementCount);
    return *this;
}

template <>
const Vector<double>& Vector<double>::add(const Vector<double>& input) const
{
    vDSP_vaddD((const double*)input.data, 1, (const double*)data, 1,
               (double*)data, 1, elementCount);
    return *this;
}

template <>
const Vector<float>& Vector<float>::hanningWindow() const
{
    float* data = (float*)this->data;
    vDSP_hann_window(data, this->elementCount, vDSP_HANN_DENORM);
    return *this;
}

template <>
const Vector<double>& Vector<double>::hanningWindow() const
{
    double* data = (double*)this->data;
    vDSP_hann_windowD(data, this->elementCount, vDSP_HANN_DENORM);
    return *this;
}

template <typename T>
const Vector<T>& Vector<T>::shift(int amount) const
{
    if (abs(amount) >= elementCount) {
        fill(0);
    }
    else if (amount > 0) {
        const size_t count = elementCount - amount;
        memcpy((void*)&data[amount], (const void*)data, count * sizeof(T));
        this->sub(abs(amount), 0).fill(0);
    }
    else if (amount < 0) {
        const size_t count = elementCount + amount;
        memcpy((void*)data, (const void*)&data[abs(amount)], count * sizeof(T));
        this->sub(abs(amount), count).fill(0);
    }
    return *this;
}

template <typename T>
const Vector<T> Vector<T>::sub(size_t length, size_t offset) const
{
    if (length + offset > elementCount) {
        cout << "Vector::sub error, invalid length and offset combination" << endl;
        exit(-1);
    }

    return Vector<T>((T*)&data[offset], length);
}

template <typename T>
void Vector<T>::print() const
{
    for (size_t i = 0; i < elementCount; ++i) {
        cout << "[" << data[i] << "]";
    }
    cout << endl;
}
template <typename T>
void Vector<T>::copy(const Vector<T>& from, const Vector<T>& to)
{
    if (from.elementCount != to.elementCount) {
        cout << "Vector copy: vectors are not the same size" << endl;
        exit(-1);
    }

    memcpy((void*)to.data, (const void*)from.data, sizeof(T) * from.elementCount);
}

template <typename T>
T Vector<T>::operator[](const size_t index) const
{
    assert(index < elementCount);
    return this->data[index];
}

template <typename T>
Matrix<T>::Matrix(function<T*(size_t input)> memoryAllocator, const size_t rowCount, const size_t columnCount)
    : Vector<T>(memoryAllocator, rowCount * columnCount),
      rowCount(rowCount),
      columnCount(columnCount)
{
}

template <typename T>
Matrix<T>::Matrix(T* data, const size_t rowCount, const size_t columnCount)
    : Vector<T>(data, rowCount * columnCount), rowCount(rowCount), columnCount(columnCount)
{
}

template <typename T>
Matrix<T>::~Matrix()
{
}

template <typename T>
Vector<T> Matrix<T>::operator[](const size_t index) const
{
    assert(index < rowCount);
    return Vector<T>((T*)&this->data[index * columnCount], columnCount);
}

template class Vector<double>;
template class Vector<float>;
template class Matrix<double>;
template class Matrix<float>;
template class VectorFactory<double>;
template class VectorFactory<float>;
