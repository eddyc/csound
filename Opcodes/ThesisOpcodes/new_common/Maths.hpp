#include "Vector.hpp"

template <typename T>
void vecVFill(const T value, const T* data, size_t elementCount);

template <typename T>
T vecVSum(const T* data, size_t elementCount);

template <typename T>
void vecVAdd(const T* a, const T* b, T* output, size_t elementCount);

template <typename T>
void vecVSubtract(const T* a, const T* b, T* output, size_t elementCount);

template <typename T>
void vecVDivide(const T* a, const T* b, T* output, size_t elementCount);

template <typename T>
void vecSAdd(const T* a, const T scalar, T* output, size_t elementCount);

template <typename T>
void vecSDivide(const T* a, const T scalar, T* output, size_t elementCount);

template <typename T>
void vecVPower(const T* base, const T* exponent, T* output, size_t elementCount);

template <typename T>
void vecVMagnitude(const T* real, const T* imag, T* output, size_t elementCount);

template <typename T>
void vecVMagnitudeSquared(const T* real, const T* imag, T* output, size_t elementCount);

template <typename T>
void vecVLog10(const T* input, T* output, size_t elementCount);

template <typename T>
void matMMultiply(const T* a, const T* b, T* c, size_t ACRowCount, size_t BCColumnCount, size_t AColumnBRowCount);
