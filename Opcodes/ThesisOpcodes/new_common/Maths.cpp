#include "Maths.hpp"
#include "UseAccelerate.hpp"
#include <math.h>
#if defined(__USE_ACCELERATE__)
#include <Accelerate/Accelerate.h>
#endif

#pragma mark vecVFill
template <typename T>
void vecVFill(const T value, const T* data, size_t elementCount)
{
#if defined(__USE_ACCELERATE__)
    if constexpr (sizeof(T) == sizeof(double)) {
        vDSP_vfillD(&value, (double*)data, 1, elementCount);
    } else if constexpr (sizeof(T) == sizeof(float)) {
        vDSP_vfill(&value, (float*)data, 1, elementCount);
    }
#else
    for (size_t i = 0; i < elementCount; ++i) {
        data[i] = value;
    }
#endif
}

template void vecVFill(const double value, const double* data, size_t elementCount);
template void vecVFill(const float value, const float* data, size_t elementCount);

#pragma mark vecVSum
template <typename T>
T vecVSum(const T* data, size_t elementCount)
{
    T result = 0;
#if defined(__USE_ACCELERATE__)
    if constexpr (sizeof(T) == sizeof(double)) {
        vDSP_sveD(data, 1, &result, elementCount);

    } else if constexpr (sizeof(T) == sizeof(float)) {
        vDSP_sve(data, 1, &result, elementCount);
    }
#else
    for (size_t i = 0; i < elementCount; ++i) {
        result += data[i];
    }
#endif
    return result;
}

template double vecVSum(const double* data, size_t elementCount);
template float vecVSum(const float* data, size_t elementCount);

#pragma mark vecVAdd
template <typename T>
void vecVAdd(const T* a, const T* b, T* output, size_t elementCount)
{
#if defined(__USE_ACCELERATE__)
    if constexpr (sizeof(T) == sizeof(double)) {
        vDSP_vaddD(a, 1, b, 1, output, 1, elementCount);
    } else if constexpr (sizeof(T) == sizeof(float)) {
        vDSP_vadd(a, 1, b, 1, output, 1, elementCount);
    }
#else
    for (size_t i = 0; i < elementCount; ++i) {
        output[i] = a[i] + b[i];
    }
#endif
}

template void vecVAdd(const double* a, const double* b, double* output, size_t elementCount);
template void vecVAdd(const float* a, const float* b, float* output, size_t elementCount);

#pragma mark vecVSubtract
template <typename T>
void vecVSubtract(const T* a, const T* b, T* output, size_t elementCount)
{
#if defined(__USE_ACCELERATE__)
    if constexpr (sizeof(T) == sizeof(double)) {
        vDSP_vsubD(a, 1, b, 1, output, 1, elementCount);
    } else if constexpr (sizeof(T) == sizeof(float)) {
        vDSP_vsub(a, 1, b, 1, output, 1, elementCount);
    }
#else
    for (size_t i = 0; i < elementCount; ++i) {
        output[i] = a[i] - b[i];
    }
#endif
}

template void vecVSubtract(const double* a, const double* b, double* output, size_t elementCount);
template void vecVSubtract(const float* a, const float* b, float* output, size_t elementCount);

#pragma mark vecVDivide
template <typename T>
void vecVDivide(const T* a, const T* b, T* output, size_t elementCount)
{
#if defined(__USE_ACCELERATE__)
    if constexpr (sizeof(T) == sizeof(double)) {
        vDSP_vdivD(a, 1, b, 1, output, 1, elementCount);
    } else if constexpr (sizeof(T) == sizeof(float)) {
        vDSP_vdiv(a, 1, b, 1, output, 1, elementCount);
    }
#else
    for (size_t i = 0; i < elementCount; ++i) {
        output[i] = a[i] / b[i];
    }
#endif
}

template void vecVDivide(const double* a, const double* b, double* output, size_t elementCount);
template void vecVDivide(const float* a, const float* b, float* output, size_t elementCount);

#pragma mark vecSAdd
template <typename T>
void vecSAdd(const T* a, const T scalar, T* output, size_t elementCount)
{
#if defined(__USE_ACCELERATE__)
    if constexpr (sizeof(T) == sizeof(double)) {
        vDSP_vsaddD(a, 1, &scalar, output, 1, elementCount);
    } else if constexpr (sizeof(T) == sizeof(float)) {
        vDSP_vsadd(a, 1, &scalar, output, 1, elementCount);
    }
#else
    for (size_t i = 0; i < elementCount; ++i) {
        output[i] = a[i] + scalar;
    }
#endif
}

template void vecSAdd(const double* a, const double scalar, double* output, size_t elementCount);
template void vecSAdd(const float* a, const float scalar, float* output, size_t elementCount);

#pragma mark vecSDivide
template <typename T>
void vecSDivide(const T* a, const T scalar, T* output, size_t elementCount)
{
#if defined(__USE_ACCELERATE__)
    if constexpr (sizeof(T) == sizeof(double)) {
        vDSP_vsdivD(a, 1, &scalar, output, 1, elementCount);
    } else if constexpr (sizeof(T) == sizeof(float)) {
        vDSP_vsdiv(a, 1, &scalar, output, 1, elementCount);
    }
#else
    for (size_t i = 0; i < elementCount; ++i) {
        output[i] = a[i] / scalar;
    }
#endif
}

template void vecSDivide(const double* a, const double scalar, double* output, size_t elementCount);
template void vecSDivide(const float* a, const float scalar, float* output, size_t elementCount);

#pragma mark vecVInterpolate
template <typename T>
void vecVPower(const T* base, const T* exponent, T* output, size_t elementCount)
{
#if defined(__USE_ACCELERATE__)
    int count = elementCount;
    if constexpr (sizeof(T) == sizeof(double)) {
        vvpow(output, exponent, base, &count);
    } else if constexpr (sizeof(T) == sizeof(float)) {
        vvpowf(output, exponent, base, &count);
    }
#else
    for (size_t i = 0; i < elementCount; ++i) {
        output[i] = pow(base[i], exponent[i]);
    }
#endif
}

template void vecVPower(const double* base, const double* exponent, double* output, size_t elementCount);
template void vecVPower(const float* base, const float* exponent, float* output, size_t elementCount);

#pragma mark vecVMagnitude
template <typename T>
void vecVMagnitude(const T* real, const T* imag, T* output, size_t elementCount)
{
#if defined(__USE_ACCELERATE__)
    if constexpr (sizeof(T) == sizeof(double)) {
        DSPDoubleSplitComplex tempComplex = {.realp = (double*)real, .imagp = (double*)imag};
        vDSP_zvabsD(&tempComplex, 1, output, 1, elementCount);
    } else if constexpr (sizeof(T) == sizeof(float)) {
        DSPSplitComplex tempComplex = {.realp = (float*)real, .imagp = (float*)imag};
        vDSP_zvabs(&tempComplex, 1, output, 1, elementCount);
    }
#else
    for (size_t i = 0; i < elementCount; ++i) {
        output[i] = sqrt(real[i] * real[i] + imag[i] * imag[i]);
    }
#endif
}
template void vecVMagnitude(const double* real, const double* imag, double* output, size_t elementCount);
template void vecVMagnitude(const float* real, const float* imag, float* output, size_t elementCount);

#pragma mark vecVMagnitudeSquared
template <typename T>
void vecVMagnitudeSquared(const T* real, const T* imag, T* output, size_t elementCount)
{
#if defined(__USE_ACCELERATE__)
    if constexpr (sizeof(T) == sizeof(double)) {
        DSPDoubleSplitComplex tempComplex = {.realp = (double*)real, .imagp = (double*)imag};
        vDSP_zvmagsD(&tempComplex, 1, output, 1, elementCount);
    } else if constexpr (sizeof(T) == sizeof(float)) {
        DSPSplitComplex tempComplex = {.realp = (float*)real, .imagp = (float*)imag};
        vDSP_zvmags(&tempComplex, 1, output, 1, elementCount);
    }
#else
    for (size_t i = 0; i < elementCount; ++i) {
        output[i] = real[i] * real[i] + imag[i] * imag[i];
    }
#endif
}

template void vecVMagnitudeSquared(const double* real, const double* imag, double* output, size_t elementCount);
template void vecVMagnitudeSquared(const float* real, const float* imag, float* output, size_t elementCount);

#pragma mark vecVLog10
template <typename T>
void vecVLog10(const T* input, T* output, size_t elementCount)
{
#if defined(__USE_ACCELERATE__)
    const int count = elementCount;
    if constexpr (sizeof(T) == sizeof(double)) {
        vvlog10(output, input, &count);
    } else if constexpr (sizeof(T) == sizeof(float)) {
        vvlog10f(output, input, &count);
    }
#else
    for (size_t i = 0; i < elementCount; ++i) {
        output[i] = log10(input[i]);
    }
#endif
}

template void vecVLog10(const double* input, double* output, size_t elementCount);
template void vecVLog10(const float* input, float* output, size_t elementCount);

template <typename T>
void matMMultiply(const T* a, const T* b, T* c, size_t ACRowCount, size_t BCColumnCount, size_t AColumnBRowCount);

#pragma mark matMMultiply
template <typename T>
void matMMultiply(const T* a, const T* b, T* c, size_t ACRowCount, size_t BCColumnCount, size_t AColumnBRowCount)
{
#if defined(__USE_ACCELERATE__)
    if constexpr (sizeof(T) == sizeof(double)) {
        vDSP_mmulD(a, 1, b, 1, c, 1, ACRowCount, BCColumnCount, AColumnBRowCount);
    } else if constexpr (sizeof(T) == sizeof(float)) {
        vDSP_mmul(a, 1, b, 1, c, 1, ACRowCount, BCColumnCount, AColumnBRowCount);
    }
#else
    for (size_t i = 0; i < ACRowCount; i++) {
        for (size_t j = 0; j < BCColumnCount; j++) {
            c[i * BCColumnCount + j] = 0;
            for (size_t k = 0; k < AColumnBRowCount; k++) {
                c[i * BCColumnCount + j] +=
                    a[i * AColumnBRowCount + k] * b[k * BCColumnCount + j];
            }
        }
    }
#endif
}

template void matMMultiply(const double* a,
                           const double* b,
                           double* c,
                           size_t ACRowCount,
                           size_t BCColumnCount,
                           size_t AColumnBRowCount);
template void matMMultiply(const float* a,
                           const float* b,
                           float* c,
                           size_t ACRowCount,
                           size_t BCColumnCount,
                           size_t AColumnBRowCount);
