#include "Interpolator.hpp"
#include <math.h>
#ifdef __USE_ACCELERATE__
#include <Accelerate/Accelerate.h>
#endif
using namespace std;
template <typename T>
Interpolator<T>::Interpolator(function<T*(size_t)> allocator, const Vector<T>& indices)
    : sortedIndices(allocator, indices.elementCount)
{
    Vector<T>::copy(indices, sortedIndices);
    sort(sortedIndices.data, sortedIndices.data + sortedIndices.elementCount);
}

template <typename T>
Interpolator<T>::Interpolator()
{
}

template <typename T>
Interpolator<T>::~Interpolator()
{
}

#ifdef __USE_ACCELERATE__
template <typename T>
void Interpolator<T>::process(Vector<T>& input, Vector<T>& output)
{
    if (input.elementCount != output.elementCount ||
        input.elementCount != sortedIndices.elementCount) {
        cout << "Interpolator, inputs for process not sane" << endl;
        exit(-1);
    }
}
#else
template <typename T>
void Interpolator<T>::operator()(const Vector<T>& input, Vector<T>& output)
{
    if (input.elementCount != output.elementCount ||
        input.elementCount != sortedIndices.elementCount || &input == &output) {
        cout << "Interpolator, inputs for process not sane" << endl;
        exit(-1);
    }

    for (size_t i = 0; i < input.elementCount; ++i) {
        size_t newIndex = sortedIndices[i];
        size_t startIndex = floor(newIndex);
        size_t endIndex = ceil(newIndex);

        if (newIndex < 0 || newIndex > input.elementCount - 1) {
            output.data[i] = NAN;
        }
        else {
            T modulo = fmod(sortedIndices[i], 1);
            T startValue = input[startIndex];
            T endValue = input[endIndex];
            T result = startValue + ((endValue - startValue) * modulo);
            output.data[i] = result;
        }
    }
}
#endif

template class Interpolator<double>;
template class Interpolator<float>;
