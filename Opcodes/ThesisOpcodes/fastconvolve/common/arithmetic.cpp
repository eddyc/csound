#include "arithmetic.hpp"

void fillAccelerate(float* data, size_t length, float value)
{
    vDSP_vfill(data, &value, 1, length);
}

void fillAccelerate(double* data, size_t length, double value)
{
    vDSP_vfillD(data, &value, 1, length);
}

template <typename T>
void fillStandard(T* data, size_t length, T value)
{
    for (size_t i = 0; i < length; ++i) {
        data[i] = value;
    }
}

template void fillStandard<double>(double*, size_t, double);
template void fillStandard<float>(float*, size_t, float);
