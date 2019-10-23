#include "Maths.hpp"
#include <math.h>
#if defined(__USE_ACCELERATE__)
#include <Accelerate/Accelerate.h>
#endif

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
