//
//
//  VectorMaths.cpp
//  Signals
//
//  Created by Edward Costello on 15/01/2018.
//  Copyright © 2018 Edward Costello. All rights reserved.
//

#include "Matrix.hpp"
#include "Vector.hpp"
#include <Accelerate/Accelerate.h>
#include <assert.h>
#include <math.h>

using namespace Signals;

template <typename T>
Vector<T>& Vector<T>::fill(const T input)
{
    T* data = (T*)this->data;
    for (size_t i = 0; i < this->elementCount; ++i) {
        data[i] = input;
    }
    return *this;
}

template <typename T>
Vector<T>& Vector<T>::clear()
{
    T* data = (T*)this->data;
    memset(data, 0, sizeof(T) * elementCount);
    return *this;
}

template <typename T>
Vector<T>& Vector<T>::multiply(const T input)
{
    T* data = (T*)this->data;
    for (size_t i = 0; i < this->elementCount; ++i) {
        data[i] *= input;
    }
    return *this;
}

template <>
Vector<double>& Vector<double>::multiply(const double input)
{
    double* data = (double*)this->data;
    vDSP_vsmulD(data, 1, &input, data, 1, this->elementCount);
    return *this;
}

template <>
Vector<double>& Vector<double>::divide(const double input)
{
    double* data = (double*)this->data;
    vDSP_vsdivD(data, 1, &input, data, 1, this->elementCount);
    return *this;
}

template <typename T>
Vector<T>& Vector<T>::add(const T input)
{
    T* data = (T*)this->data;
    for (size_t i = 0; i < this->elementCount; ++i) {
        data[i] += input;
    }
    return *this;
}

template <typename T>
Vector<T>& Vector<T>::add(StridedVector<T>& input)
{
    T* data = (T*)this->data;
    for (size_t i = 0; i < this->elementCount; ++i) {
        data[i] += input[i];
    }
    return *this;
}

template <typename T>
Vector<T>& Vector<T>::exponent()
{
    T* data = (T*)this->data;
    for (size_t i = 0; i < this->elementCount; ++i) {
        data[i] = exp(data[i]);
    }
    return *this;
}

template <>
Vector<double>& Vector<double>::exponent()
{
    double* data = (double*)this->data;
    int elements = this->elementCount;
    vvexp(data, (const double*)data, &elements);

    return *this;
}

template <typename T>
Vector<T>& Vector<T>::cosine()
{
    T* data = (T*)this->data;

    for (size_t i = 0; i < this->elementCount; ++i) {
        data[i] = cos(data[i]);
    }
    return *this;
}

template <>
Vector<double>& Vector<double>::cosine()
{
    double* data = (double*)this->data;
    int elements = this->elementCount;
    vvcos(data, (const double*)data, &elements);
    return *this;
}

template <typename T>
Vector<T>& Vector<T>::reverse()
{
    for (size_t i = 0; i < elementCount / 2; ++i) {
        T* data = (T*)this->data;
        T x = data[i];
        data[i] = data[elementCount - 1 - i];
        data[elementCount - 1 - i] = x;
    }

    return *this;
}

template <typename T>
void Vector<T>::add(const Vector<T>& inputA, const Vector<T>& inputB, Vector<T>& output)
{
    assert(inputA.elementCount == inputB.elementCount);
    assert(inputA.elementCount == output.elementCount);

    T* outData = (T*)output.data;
    for (size_t i = 0; i < inputA.elementCount; ++i) {
        outData[i] = inputA.data[i] + inputB.data[i];
    }
}

template <typename T>
void Vector<T>::divide(const Vector<T>& inputA, const T& scalar, Vector<T>& output)
{
    assert(inputA.elementCount == output.elementCount);

    T* outData = (T*)output.data;
    for (size_t i = 0; i < inputA.elementCount; ++i) {
        outData[i] = inputA.data[i] / scalar;
    }
}

template <typename T>
void Vector<T>::multiply(const Vector<T>& input, const T& scalar, Vector<T>& output)
{
    T* outData = (T*)output.data;
    for (size_t i = 0; i < input.elementCount; ++i) {
        outData[i] = input.data[i] * scalar;
    }
}

template <>
void Vector<double>::multiply(const Vector<double>& input,
                              const double& scalar,
                              Vector<double>& output)
{
    double* outData = (double*)output.data;
    vDSP_vsmulD(input.data, 1, &scalar, outData, 1, output.elementCount);
}

template <typename T>
void Vector<T>::multiply(const Vector<T>& inputA, const Vector<T>& inputB, Vector<T>& output)
{
    assert(inputA.elementCount == inputB.elementCount);
    assert(inputA.elementCount == output.elementCount);

    T* outData = (T*)output.data;
    for (size_t i = 0; i < inputA.elementCount; ++i) {
        outData[i] = inputA.data[i] * inputB.data[i];
    }
}

template <typename T>
void Vector<T>::subtract(const StridedVector<T>& inputA, const Vector<T>& inputB, Vector<T>& output)
{
    assert(inputA.elementCount == inputB.elementCount);
    assert(inputA.elementCount == output.elementCount);

    T* outData = (T*)output.data;
    for (size_t i = 0; i < inputA.elementCount; ++i) {
        outData[i] = inputA[i] - inputB.data[i];
    }
}

template <typename T>
void Vector<T>::subtract(Signals::Vector<T>& input, double scalar, Signals::Vector<T>& output)
{
    assert(input.elementCount == output.elementCount);

    T* outData = (T*)output.data;
    T* inData = (T*)input.data;
    for (size_t i = 0; i < input.elementCount; ++i) {
        outData[i] = inData[i] - scalar;
    }
}

template <typename T>
void Vector<T>::subtract(Vector<T>& inputA, Vector<T>& inputB, Vector<T>& output)
{
    assert(inputA.elementCount == inputB.elementCount);
    assert(inputA.elementCount == output.elementCount);

    T* outData = (T*)output.data;
    for (size_t i = 0; i < inputA.elementCount; ++i) {
        outData[i] = inputA.data[i] - inputB.data[i];
    }
}

template <>
void Vector<double>::subtract(Vector<double>& inputA, Vector<double>& inputB, Vector<double>& output)
{
    assert(inputA.elementCount == inputB.elementCount);
    assert(inputA.elementCount == output.elementCount);

    double* outData = (double*)output.data;
    vDSP_vsubD(inputB.data, 1, inputA.data, 1, outData, 1, output.elementCount);
}

template <typename T>
T Vector<T>::dotProduct(const Vector<T>& inputA, const Vector<T>& inputB)
{
    assert(inputA.elementCount == inputB.elementCount);

    T output = 0;
    for (size_t i = 0; i < inputA.elementCount; ++i) {
        output += inputA.data[i] * inputB.data[i];
    }

    return output;
}

template <>
double Vector<double>::dotProduct(const Vector<double>& inputA, const Vector<double>& inputB)
{
    assert(inputA.elementCount == inputB.elementCount);
    double output;
    vDSP_dotprD(inputA.data, 1, inputB.data, 1, &output, inputA.elementCount);

    return output;
}

template <typename T>
void Vector<T>::outerProduct(const Vector<T>& inputA, const Vector<T>& inputB, Matrix<T>& output)
{
    assert(inputA.elementCount == inputB.elementCount);
    assert(inputA.elementCount == output.rowCount);
    assert(inputA.elementCount == output.columnCount);

    for (size_t i = 0; i < inputA.elementCount; ++i) {
        for (size_t j = 0; j < inputB.elementCount; ++j) {
            output[i][j] = inputA.data[i] * inputB.data[j];
        }
    }
}

template <typename T>
Vector<T>& Vector<T>::ramp(const T start, const T increment)
{
    T* data = (T*)this->data;
    for (size_t i = 0; i < this->elementCount; ++i) {
        data[i] = start + increment * i;
    }
    return *this;
}

template <typename T>
Vector<T>& Vector<T>::sineWindow()
{
    T* data = (T*)this->data;
    for (size_t i = 0; i < this->elementCount; ++i) {
        data[i] = sin((((Float64)i + 1.) - 0.5) * M_PI / this->elementCount);
    }
    return *this;
}

template <>
Vector<float>& Vector<float>::hanningWindow()
{
    float* data = (float*)this->data;
    vDSP_hann_window(data, this->elementCount, vDSP_HANN_DENORM);
    return *this;
}

template <>
Vector<double>& Vector<double>::hanningWindow()
{
    double* data = (double*)this->data;
    vDSP_hann_windowD(data, this->elementCount, vDSP_HANN_DENORM);
    return *this;
}

template class Signals::Vector<double>;
template class Signals::Vector<float>;
