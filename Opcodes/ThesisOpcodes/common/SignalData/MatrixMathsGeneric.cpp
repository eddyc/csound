//
//  MatrixMaths.cpp
//  Signals
//
//  Created by Edward Costello on 15/01/2018.
//  Copyright © 2018 Edward Costello. All rights reserved.
//

#include "Matrix.hpp"
#include <math.h>
#include <assert.h>

extern "C" {
#include <f2c.h>
#include <clapack.h>
}

using namespace Signals;

template<typename T>
Matrix<T> &Matrix<T>::identity() {
    
    for (size_t i = 0; i < rowCount; ++i) {
        
        (*this)[i][i] = 1;
    }
    return *this;
}

//template<typename T>
//Matrix<T> &Matrix<T>::choleskyFactorisation() {
//
//    auto divide = [](T *data, T scalar, size_t stride, size_t elementCount) {
//
//        for (size_t i = 0; i < elementCount; ++i) {
//            data[i * stride] /= scalar;
//        }
//    };
//
//    for (size_t i = 0; i < rowCount; ++i) {
//
//
//        (*this)[i][i] = sqrt((*this)[i][i]);
//        (*this)[i].subVector(i + 1, rowCount - i - 1).clear();
//        T *data = (T *)this->data;
//
//        divide((T *)&data[((i + 1) * columnCount) + i],
//               (*this)[i][i], columnCount,
//               (size_t)(rowCount - i - 1));
//
//        for (size_t j = i + 1; j < rowCount; ++j) {
//
//            for (size_t k = 0; k < rowCount - i - 1; ++k) {
//
//                T scalar = (*this)[j][i];
//                Vector<T> vec = (*this)[i + 1];
//                T *data = (T *)vec.data;
//                scalar *= data[i + k * columnCount];
//                data[j + k * columnCount] -= scalar;
//            }
//        }
//    }
//    return *this;
//}


template<typename T>
Matrix<T> &Matrix<T>::choleskyFactorisation() {
    
//    auto divide = [](T *data, T scalar, size_t stride, size_t elementCount) {
//        
//        for (size_t i = 0; i < elementCount; ++i) {
//            data[i * stride] /= scalar;
//        }
//    };
    
    T *data = (T *)this->data;
    
    for (size_t i = 0; i < rowCount; ++i) {
        
        
//        (*this)[i][i] = sqrt((*this)[i][i]);
        data[i * columnCount + i] = sqrt(data[i * columnCount + i]);
        
//        (*this)[i].subVector(i + 1, rowCount - i - 1).clear();
        memset(&data[i * columnCount + i + 1], 0, sizeof(T) * (rowCount - i - 1));

//        divide((T *)&data[((i + 1) * columnCount) + i],
//               (*this)[i][i], columnCount,
//               (size_t)(rowCount - i - 1));
        
        for (size_t x = 0; x < rowCount - i - 1; ++x) {
            
            data[(((i + 1) * columnCount) + i) + x * columnCount] /= data[i * columnCount + i];
        }
        
        for (size_t j = i + 1; j < rowCount; ++j) {
            

            for (size_t k = 0; k < rowCount - i - 1; ++k) {
                
                T scalar = data[j * columnCount + i];
                T *dat = &data[((i + 1) * columnCount)];
                scalar *= dat[i + k * columnCount];
                dat[j + k * columnCount] -= scalar;
            }
        }
    }
    return *this;
}


template<typename T>
Matrix<T> &Matrix<T>::choleskyFactorisation2() {

    char U = 'L';
    integer order = this->rowCount;
    integer lda = this->columnCount;
    integer info = 0;
    dpotf2_(&U, &order, (double *)this->data, &lda, &info);
    return *this;
}

template<typename T>
Matrix<T> &Matrix<T>::multiply(const T input) {
    
    T *data = (T *)this->data;
    for (size_t i = 0; i < this->elementCount; ++i) {
        
        data[i] *= input;
    }
    return *this;
}


//template<typename T>
//void Matrix<T>::multiply(const Matrix<T> &inputA, const Matrix<T> &inputB, Matrix<T> &output)
//{
//    assert(inputA.columnCount == inputB.rowCount);
//    assert(output.rowCount == inputA.rowCount);
//    assert(output.columnCount == inputB.columnCount);
//
//    for (size_t i = 0; i < inputA.rowCount; i++) {
//
//        for (size_t j = 0; j < inputB.columnCount; j++) {
//
//            output[i][j] = 0;
//
//            for (size_t k = 0; k < inputA.columnCount; k++){
//
//                output[i][j] += inputA[i][k] * inputB[k][j];
//            }
//        }
//    }
//}

void dgemm(...) {
    
}

template <>
void Matrix<double>::multiply(const Matrix<double> &inputA, const Matrix<double> &inputB, Matrix<double> &output)
{
    assert(inputA.columnCount == inputB.rowCount);
    assert(output.rowCount == inputA.rowCount);
    assert(output.columnCount == inputB.columnCount);
    
    long int m = inputA.rowCount;
    long int n = inputB.columnCount;
    long int k = inputA.columnCount;
    
    double alph = 1;
    double bet = 0;
    double *aData = (double *)inputA.data;
    double *bData = (double *)inputB.data;
    double *outData = (double *)output.data;
    
    char N[] = "N";
    dgemm_((char *)N, (char *)N, &n, &m, &k, &alph, bData, &n, aData, &k, &bet, outData, &n);
}


template <>
void Matrix<double>::multiplyTransposed(const Matrix<double> &inputA,
                                        const Matrix<double> &inputB,
                                        Matrix<double> &output)
{
    
    assert(inputA.columnCount == inputB.columnCount);
    assert(output.rowCount == inputA.rowCount);
    assert(output.columnCount == inputB.rowCount);

    long int m =  inputB.rowCount;
    long int n =  inputA.rowCount;
    long int k =  inputA.columnCount;
    
    long int lda = inputB.columnCount;
    long int ldb = inputA.columnCount;
    long int ldc = inputB.rowCount;
    
    double alph = 1;
    double beta = 0;
    double *a = (double *)inputA.data;
    double *b = (double *)inputB.data;
    double *c = (double *)output.data;
    
    char N[] = "N";
    char T[] = "T";
    
    dgemm_(T, N, &m, &n, &k, &alph, b, &ldb, a, &lda, &beta, c, &ldc);
}

template<typename T>
bool Matrix<T>::equals(const Matrix<T> other)
{
    assert(this->rowCount == other.rowCount);
    assert(this->columnCount == other.columnCount);
    assert(this->elementCount == other.elementCount);
    
    for (size_t i = 0; i < other.elementCount; ++i) {
        if (this->data[i] != other.data[i]) {
            return false;
        }
    }
    
    return true;
}

template class Signals::Matrix<double>;
template class Signals::Matrix<float>;
