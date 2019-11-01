#include "Detrender.hpp"
#include "UseAccelerate.hpp"
#include <math.h>
#ifdef __USE_ACCELERATE__
#include <Accelerate/Accelerate.h>
#endif
using namespace std;
template <typename T>
Detrender<T>::Detrender(function<T*(size_t)> allocator, const size_t binCount)
    : lbp(2),
      binCount(binCount),
      work(allocator, lbp * binCount),
      inputCopy1(allocator, 1, binCount),
      inputCopy2(allocator, 1, binCount),
      setA1(allocator, lbp, binCount),
      a1(allocator, binCount, lbp),
      a2(allocator, binCount, lbp),
      a3(allocator, binCount, 1),
      a4(allocator, lbp, 1)

{
    setA1.sub(binCount, 0).ramp(1, binCount).divide(binCount);
    setA1.sub(binCount, binCount).fill(1);
    Vector<T>::copy(setA1[0], a2.sub(a2.elementCount - 1, 0), 2);
    Vector<T>::copy(setA1.sub(binCount, binCount), a2.sub(a2.elementCount - 1, 1), 2);
}

template <typename T>
Detrender<T>::Detrender()
{
}

template <typename T>
Detrender<T>::~Detrender()
{
}

#ifdef __USE_ACCELERATE__
template <typename T>
void Detrender<T>::operator()(const Vector<T>& input, Vector<T>& output)
{
    if (input.elementCount != output.elementCount || input.elementCount != binCount) {
        cout << "Detrender, inputs for process not sane" << endl;
        exit(-1);
    }
    Vector<T>::copy(input, inputCopy1);
    Vector<T>::copy(input, inputCopy2);
    Vector<T>::copy(setA1, a1);
    char trans = 'N';
    int n = 2;
    int m = (int)input.elementCount;
    int nrhs = 1;
    int lwork = (int)(lbp * input.elementCount);
    int info = 0;

    if constexpr (sizeof(T) == sizeof(double)) {
        dgels_(&trans, &m, &n, &nrhs, (T*)a1.data, &m, (T*)inputCopy2.data, &m,
               (T*)work.data, &lwork, &info);

    } else {
        sgels_(&trans, &m, &n, &nrhs, (T*)a1.data, &m, (T*)inputCopy2.data, &m,
               (T*)work.data, &lwork, &info);
    }

    Vector<T>::copy(inputCopy2.sub(lbp, 0), a4);
    Matrix<T>::multiply(a2, a4, a3);
    Vector<T>::subtract(a3, inputCopy1, output);
}
#else
template <typename T>
void Detrender<T>::operator()(const Vector<T>& input, Vector<T>& output)
{
    if (input.binCount != output.binCount ||
        input.binCount != sortedIndices.binCount || &input == &output) {
        cout << "Detrender, inputs for process not sane" << endl;
        exit(-1);
    }
}
#endif

template class Detrender<double>;
template class Detrender<float>;
