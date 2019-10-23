#include "Vector.hpp"
#define __USE_ACCELERATE__

template <typename T>
void vecVAdd(const T* a, const T* b, T* output, size_t elementCount);

template <typename T>
void vecSAdd(const T* a, const T scalar, T* output, size_t elementCount);

template <typename T>
void vecVPower(const T* base, const T* exponent, T* output, size_t elementCount);

template <typename T>
void vecVMagnitude(const T* real, const T* imag, T* output, size_t elementCount);

template <typename T>
void vecVMagnitudeSquared(const T* real, const T* imag, T* output, size_t elementCount);
