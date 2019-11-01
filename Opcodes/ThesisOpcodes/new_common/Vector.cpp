#include "Vector.hpp"
#include "Maths.hpp"
#include "Socket.hpp"
#include <Accelerate/Accelerate.h>

using namespace std;
template <typename T>
Vector<T>::Vector(function<T*(size_t input)> memoryAllocator, size_t elementCount)
    : data(memoryAllocator(elementCount)), elementCount(elementCount)
{
}

template <typename T>
Vector<T>::Vector() : data(nullptr), elementCount(0)
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
    } else {
        const size_t remainder = elementCount - length;
        memcpy((void*)data, (void*)&data[length], sizeof(T) * remainder);
        memcpy((void*)&data[remainder], (const void*)input.data,
               sizeof(T) * input.elementCount);
    }
    return *this;
}

template <typename T>
const Vector<T>& Vector<T>::clear() const
{
    memset((void*)data, 0, sizeof(T) * elementCount);
    return *this;
}

template <typename T>
const T Vector<T>::sum() const
{
    return vecVSum(data, elementCount);
}

template <typename T>
const T Vector<T>::sum(const size_t offset, const size_t count) const
{
    assert(offset + count <= elementCount);
    return vecVSum(&data[offset], count);
}

template <typename T>
const Vector<T>& Vector<T>::fill(const T value) const
{
    vecVFill(value, data, elementCount);
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

template <typename T>
const Vector<T>& Vector<T>::power(const Vector<T>& power) const
{
    vecVPower(data, power.data, data, elementCount);
    return *this;
}

template <typename T>
void Vector<T>::power(const Vector<T>& base, const Vector<T>& exponent, const Vector<T>& output)
{
    assert(base.elementCount == output.elementCount);
    assert(output.elementCount == exponent.elementCount);
    vecVPower(base.data, exponent.data, output.data, output.elementCount);
}

template <>
const Vector<double>& Vector<double>::reverse() const
{
    vDSP_vrvrsD((double*)data, 1, elementCount);
    return *this;
}

template <>
const Vector<float>& Vector<float>::reverse() const
{
    vDSP_vrvrs((float*)data, 1, elementCount);
    return *this;
}

template <typename T>
const Vector<T>& Vector<T>::log10() const
{
    vecVLog10(data, data, elementCount);
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

template <typename T>
void Vector<T>::divide(const Vector<T>& a, const Vector<T>& b, const Vector<T>& output)
{
    assert(a.elementCount == b.elementCount && a.elementCount == output.elementCount);
    vecVDivide(a.data, b.data, output.data, output.elementCount);
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

template <typename T>
void Vector<T>::add(const Vector<T>& a, const Vector<T>& b, const Vector<T>& output)
{
    if (a.elementCount != b.elementCount || a.elementCount != output.elementCount) {
        cout << "Vector multiply: vectors are not the same size" << endl;
        exit(-1);
    }

    vecVAdd(a.data, b.data, output.data, a.elementCount);
}

template <typename T>
const Vector<T>& Vector<T>::add(const Vector<T>& input) const
{
    vecVAdd(input.data, data, data, elementCount);
    return *this;
}

template <typename T>
void Vector<T>::subtract(const Vector<T>& a, const Vector<T>& b, const Vector<T>& output)
{
    if (a.elementCount != b.elementCount || a.elementCount != output.elementCount) {
        cout << "Vector subtract: vectors are not the same size" << endl;
        exit(-1);
    }

    vecVSubtract(a.data, b.data, output.data, a.elementCount);
}

template <typename T>
const Vector<T>& Vector<T>::subtract(const Vector<T>& input) const
{
    vecVSubtract(input.data, data, data, elementCount);
    return *this;
}

template <typename T>
const Vector<T>& Vector<T>::add(const T input) const
{
    vecSAdd(data, input, data, elementCount);
    return *this;
}

template <typename T>
const Vector<T>& Vector<T>::divide(const T input) const
{
    vecSDivide(data, input, data, elementCount);
    return *this;
}

template <>
void Vector<double>::angle(const Vector<double>& real,
                           const Vector<double>& imag,
                           const Vector<double>& angle)
{
    if (real.elementCount != imag.elementCount || real.elementCount != angle.elementCount) {
        cout << "Vector angle: vectors are not the same size" << endl;
        exit(-1);
    }

    DSPDoubleSplitComplex input = {
        .realp = (double*)real.data,
        .imagp = (double*)imag.data,
    };

    vDSP_zvphasD(&input, 1, (double*)angle.data, 1, angle.elementCount);
}

template <>
void Vector<float>::angle(const Vector<float>& real,
                          const Vector<float>& imag,
                          const Vector<float>& angle)
{
    if (real.elementCount != imag.elementCount || real.elementCount != angle.elementCount) {
        cout << "Vector angle: vectors are not the same size" << endl;
        exit(-1);
    }

    DSPSplitComplex input = {
        .realp = (float*)real.data,
        .imagp = (float*)imag.data,
    };
    vDSP_zvphas(&input, 1, (float*)angle.data, 1, angle.elementCount);
}

template <typename T>
void Vector<T>::magnitude(const Vector<T>& real, const Vector<T>& imag, const Vector<T>& magnitude)
{
    if (real.elementCount != imag.elementCount || real.elementCount != magnitude.elementCount) {
        cout << "Vector magnitude: vectors are not the same size" << endl;
        exit(-1);
    }

    vecVMagnitude(real.data, imag.data, magnitude.data, real.elementCount);
}

template <typename T>
void Vector<T>::magnitudeSquared(const Vector<T>& real,
                                 const Vector<T>& imag,
                                 const Vector<T>& magnitude)
{
    if (real.elementCount != imag.elementCount || real.elementCount != magnitude.elementCount) {
        cout << "Vector magnitude: vectors are not the same size" << endl;
        exit(-1);
    }

    vecVMagnitudeSquared(real.data, imag.data, magnitude.data, real.elementCount);
}

template <typename T>
void Vector<T>::interpolate(const Vector<T>& input,
                            const Vector<T>& indices,
                            const Vector<T>& output)
{
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

template <>
const Vector<float>& Vector<float>::sineWindow() const
{
    float* data = (float*)this->data;
    for (size_t i = 0; i < elementCount; ++i) {
        data[i] = sin((((float)i + 1.) - 0.5) * M_PI / elementCount);
    }
    return *this;
}

template <>
const Vector<double>& Vector<double>::sineWindow() const
{
    double* data = (double*)this->data;
    for (size_t i = 0; i < elementCount; ++i) {
        data[i] = sin((((double)i + 1.) - 0.5) * M_PI / elementCount);
    }
    return *this;
}

template <typename T>
const Vector<T>& Vector<T>::shift(int amount) const
{
    if (abs(amount) >= elementCount) {
        fill(0);
    } else if (amount > 0) {
        const size_t count = elementCount - amount;
        memcpy((void*)&data[amount], (const void*)data, count * sizeof(T));
        this->sub(abs(amount), 0).fill(0);
    } else if (amount < 0) {
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
void Vector<T>::copy(const Vector<T>& from, const Vector<T>& to, const size_t stride)
{
    if (to.elementCount != from.elementCount * stride - 1) {
        cout << "Vector copy: vectors are required the same size" << endl;
        exit(-1);
    }

    for (size_t i = 0; i < from.elementCount; ++i) {
        to[stride * i] = from[i];
    }
}

template <typename T>
inline T& Vector<T>::operator[](const size_t index) const
{
    assert(index < elementCount);
    return this->data[index];
}

template <typename T>
ResizableVector<T>::ResizableVector(function<T*(size_t input)> allocator,
                                    const size_t allocatedElementCount)
    : Vector<T>(allocator, allocatedElementCount),
      allocatedElementCount(allocatedElementCount)
{
}

template <typename T>
ResizableVector<T>::ResizableVector() : Vector<T>(), allocatedElementCount(0)
{
}

template <typename T>
ResizableVector<T>::~ResizableVector()
{
}

template <typename T>
void ResizableVector<T>::setElementCount(const size_t count) const
{
    assert(count <= allocatedElementCount);
    memcpy((void*)&this->elementCount, (const void*)&count, sizeof(size_t));
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
Matrix<T>::Matrix() : Vector<T>(), rowCount(0), columnCount(0)
{
}

template <typename T>
Matrix<T>::~Matrix()
{
}

template <typename T>
void Matrix<T>::multiply(const Matrix<T>& a, const Matrix<T>& b, const Matrix<T>& output)
{
    assert(a.rowCount == output.rowCount);
    assert(output.columnCount == b.columnCount);
    assert(a.columnCount == b.rowCount);

    matMMultiply(a.data, b.data, output.data, a.rowCount, b.columnCount, a.columnCount);
}

template <typename T>
Vector<T> Matrix<T>::operator[](const size_t index) const
{
    assert(index < rowCount);
    return Vector<T>((T*)&this->data[index * columnCount], columnCount);
}

template <typename T>
void Vector<T>::send(const string label) const
{
    if constexpr (sizeof(T) == sizeof(double)) {
        sendDataDouble((double*)data, (const char*)label.c_str(), (size_t)elementCount);
    } else {
        sendDataFloat((float*)data, (const char*)label.c_str(), (size_t)elementCount);
    }
}

template <typename T>
void Vector<T>::send(const char* label) const
{
    if constexpr (sizeof(T) == sizeof(double)) {
        sendDataDouble((double*)data, label, (size_t)elementCount);
    } else {
        sendDataFloat((float*)data, label, (size_t)elementCount);
    }
}

template class Vector<double>;
template class Vector<float>;
template class ResizableVector<double>;
template class ResizableVector<float>;
template class Matrix<double>;
template class Matrix<float>;
template class VectorFactory<double>;
template class VectorFactory<float>;
