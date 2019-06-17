//
//  MatrixMaths.cpp
//  Signals
//
//  Created by Edward Costello on 15/01/2018.
//  Copyright © 2018 Edward Costello. All rights reserved.
//

#include "Matrix.hpp"
#include <Accelerate/Accelerate.h>
#include <assert.h>
#include <math.h>

extern "C"
{
    // #include <clapack.h>
}

using namespace Signals;

template <typename T>
Matrix<T> &Matrix<T>::identity()
{

    for (size_t i = 0; i < rowCount; ++i)
    {

        (*this)[i][i] = 1;
    }
    return *this;
}

template <typename T>
Matrix<T> &Matrix<T>::choleskyFactorisation()
{

    T *data = (T *)this->data;

    for (size_t i = 0; i < rowCount; ++i)
    {

        data[i * columnCount + i] = sqrt(data[i * columnCount + i]);
        memset(&data[i * columnCount + i + 1], 0,
               sizeof(T) * (rowCount - i - 1));

        for (size_t x = 0; x < rowCount - i - 1; ++x)
        {

            data[(((i + 1) * columnCount) + i) + x * columnCount] /=
                data[i * columnCount + i];
        }

        for (size_t j = i + 1; j < rowCount; ++j)
        {

            for (size_t k = 0; k < rowCount - i - 1; ++k)
            {

                T scalar = data[j * columnCount + i];
                T *dat = &data[((i + 1) * columnCount)];
                scalar *= dat[i + k * columnCount];
                dat[j + k * columnCount] -= scalar;
            }
        }
    }
    return *this;
}

template <typename T>
Matrix<T> &Matrix<T>::choleskyFactorisation2()
{

    char U = 'L';
    int order = this->rowCount;
    int lda = this->columnCount;
    int info = 0;
    dpotf2_(&U, &order, (double *)this->data, &lda, &info);

    return *this;
}

template <typename T>
Matrix<T> &Matrix<T>::multiply(const T input)
{

    T *data = (T *)this->data;
    for (size_t i = 0; i < this->elementCount; ++i)
    {

        data[i] *= input;
    }
    return *this;
}

extern "C"
{
    extern int dgemm_(char *transa, char *transb, long int *m, long int *n,
                      long int *k, double *alpha, double *a, long int *lda,
                      double *b, long int *ldb, double *beta, double *c__,
                      long int *ldc);
}

template <>
void Matrix<double>::multiply(const Matrix<double> &inputA,
                              const Matrix<double> &inputB,
                              Matrix<double> &output)
{
    assert(inputA.columnCount == inputB.rowCount);
    assert(output.rowCount == inputA.rowCount);
    assert(output.columnCount == inputB.columnCount);

    long int m = inputA.rowCount;
    long int n = inputB.columnCount;
    long int k = inputA.columnCount;

    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, m, n, k, 1,
                (double *)&inputA.data[0], k, (double *)&inputB.data[0], n, 0,
                (double *)&output.data[0], n);
}

template <>
void Matrix<double>::multiplyTransposed(const Matrix<double> &inputA,
                                        const Matrix<double> &inputB,
                                        Matrix<double> &output)
{

    assert(inputA.columnCount == inputB.columnCount);
    assert(output.rowCount == inputA.rowCount);
    assert(output.columnCount == inputB.rowCount);

    long int m = inputA.rowCount;
    long int n = inputB.rowCount;
    long int k = inputA.columnCount;

    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasTrans, m, n, k, 1,
                (double *)&inputA.data[0], k, (double *)&inputB.data[0], k, 0,
                (double *)&output.data[0], m);
}

template <typename T>
bool Matrix<T>::equals(const Matrix<T> other)
{
    assert(this->rowCount == other.rowCount);
    assert(this->columnCount == other.columnCount);
    assert(this->elementCount == other.elementCount);

    for (size_t i = 0; i < other.elementCount; ++i)
    {
        if (this->data[i] != other.data[i])
        {
            return false;
        }
    }

    return true;
}

template class Signals::Matrix<double>;
template class Signals::Matrix<float>;
