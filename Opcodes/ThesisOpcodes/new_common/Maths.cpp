#include "Maths.hpp"
#include <math.h>
#if defined(__USE_ACCELERATE__)
#include <Accelerate/Accelerate.h>
#endif

#pragma mark vecVAdd
#if defined(__USE_ACCELERATE__)
template <>
void vecVAdd(const double* a, const double* b, double* output, size_t elementCount)
{
    vDSP_vaddD(a, 1, b, 1, output, 1, elementCount);
}

template <>
void vecVAdd(const float* a, const float* b, float* output, size_t elementCount)
{
    vDSP_vadd(a, 1, b, 1, output, 1, elementCount);
}
#else
template <typename T>
void vecVAdd(const T* a, const T* b, T* output, size_t elementCount)
{
    for (size_t i = 0; i < elementCount; ++i) {
        output[i] = a[i] + b[i];
    }
}
template void vecVAdd(const double* a, const double* b, double* output, size_t elementCount);
template void vecVAdd(const float* a, const float* b, float* output, size_t elementCount);
#endif

#pragma mark vecSAdd
#if defined(__USE_ACCELERATE__)
template <>
void vecSAdd(const double* a, const double scalar, double* output, size_t elementCount)
{
    vDSP_vsaddD(a, 1, scalar, output, 1, elementCount);
}

template <>
void vecSAdd(const float* a, const float scalar, float* output, size_t elementCount)
{
    vDSP_vadd(a, 1, scalar, output, 1, elementCount);
}
#else
template <typename T>
void vecSAdd(const T* a, const T scalar, T* output, size_t elementCount)
{
    for (size_t i = 0; i < elementCount; ++i) {
        output[i] = a[i] + scalar;
    }
}
template void vecSAdd(const double* a, const double scalar, double* output, size_t elementCount);
template void vecSAdd(const float* a, const float scalar, float* output, size_t elementCount);
#endif

#pragma mark vecVInterpolate
#if defined(__USE_ACCELERATE__)
template <>
void vecVPower(const double* base, const double* exponent, double* output, size_t elementCount)
{
    int count = elementCount;
    vvpow(output, exponent, base, &count);
}

template <>
void vecVPower(const float* base, const float* exponent, float* output, size_t elementCount)
{
    int count = elementCount;
    vvpowf(output, exponent, base, &count);
}
#else
template <typename T>
void vecVPower(const T* base, const T* exponent, T* output, size_t elementCount)
{
    for (size_t i = 0; i < elementCount; ++i) {
        output[i] = pow(base[i], exponent[i]);
    }
}
template void vecVPower(const double* base, const double* exponent, double* output, size_t elementCount);
template void vecVPower(const float* base, const float* exponent, float* output, size_t elementCount);
#endif
