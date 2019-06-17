//
//  Matrix.cpp
//  Signals
//
//  Created by Edward Costello on 15/01/2018.
//  Copyright © 2018 Edward Costello. All rights reserved.
//

#include "Matrix.hpp"
#include <assert.h>
#include <iostream>
using namespace Signals;
using namespace std;


template<typename T>
Matrix<T>::Matrix() :
Vector<T>(),
rowCount(0),
columnCount(0)
{}

template<typename T>
Matrix<T>::Matrix(tuple<T *, size_t, size_t> t) :
Vector<T>(get<0>(t), get<1>(t) * get<2>(t)),
rowCount(get<1>(t)),
columnCount(get<2>(t))
{}

template<typename T>
Matrix<T>::Matrix(T *data, size_t rowCount, size_t columnCount) :
Vector<T>(data, rowCount * columnCount),
rowCount(rowCount),
columnCount(columnCount)
{}


template<typename T>
Vector<T> Matrix<T>::operator[](size_t index) 
{
    assert(index < rowCount);
    return Vector<T>((T *)&this->data[index * columnCount], columnCount);
}

template<typename T>
void Matrix<T>::print()
{
    for (size_t i = 0; i < rowCount; ++i) {
        
        (*this)[i].print();
    }
    
    cout << endl;
}


template<typename T>
void Matrix<T>::copy( Matrix<T> input, StridedMatrix<T> output)
{
    assert(input.rowCount == output.rowCount);
    assert(input.columnCount == output.columnCount);
    
    for (size_t i = 0; i < input.rowCount; ++i) {
        
        Vector<T>::copy(input[i], output[i]);
    }
}

template<typename T>
Matrix<T> &Matrix<T>::repeatRows(const Vector<T> input)
{
    assert(input.elementCount == columnCount);

    for (size_t i = 0; i < rowCount; ++i) {
       
        Vector<T>::copy(input, (*this)[i]);
    }
    
    return *this;
}


template<typename T>
StridedMatrix<T> Matrix<T>::subMatrix(size_t rowCount, size_t rowOffset,
                                      size_t columnCount, size_t columnOffset)
{
    return StridedMatrix<T>(*this, rowCount, rowOffset, columnCount, columnOffset);
}

template<typename T>
Matrix<T> Matrix<T>::subMatrix(size_t rowCount, size_t rowOffset)
{
    assert(rowCount + rowOffset <= this->rowCount);
    return Matrix<T>((T *)&this->data[rowOffset * columnCount], rowCount, columnCount);
}

template<typename T>
StridedVector<T> Matrix<T>::diagonal()
{
    assert(rowCount == columnCount);
    return StridedVector<T>((T *)this->data, this->elementCount, rowCount, rowCount + 1);
}

template<typename T>
StridedVector<T> Matrix<T>::column(size_t index)
{
    assert(index < columnCount);
    return StridedVector<T>((T *)&this->data[index], this->elementCount - index, rowCount, columnCount);
}


template<typename T>
StridedMatrix<T>::StridedMatrix() : Vector<T>(),
rowCount(0),
columnCount(0),
stride(0)
{}

template<typename T>
StridedMatrix<T>::StridedMatrix(Matrix<T> input) : Vector<T>(input),
rowCount(input.rowCount),
columnCount(input.columnCount),
stride(input.columnCount)
{}

template<typename T>
StridedMatrix<T>::StridedMatrix(Matrix<T> input, size_t rowCount, size_t rowOffset,
                                size_t columnCount, size_t columnOffset) :
Vector<T>((T *)&input[rowOffset][columnOffset], rowCount * columnCount),
rowCount(rowCount),
columnCount(columnCount),
stride(input.columnCount)
{
    assert(rowCount + rowOffset <= input.rowCount);
    assert(columnCount + columnOffset <= input.columnCount);
}

template<typename T>
Vector<T> StridedMatrix<T>::operator[](size_t index) const
{
    assert(index < rowCount);
    return Vector<T>((T *)&this->data[index * stride], columnCount);
}


template<typename T>
void StridedMatrix<T>::print()
{
    for (size_t i = 0; i < rowCount; ++i) {
        
        for (size_t j = 0; j < columnCount; ++j) {
            
            cout << "[" << this->data[i * stride + j] << "]";
        }
        cout << endl;
    }
}

template<typename T>
void StridedMatrix<T>::copy(size_t rowCount, size_t rowOffset, size_t columnCount, size_t columnOffset, Matrix<T> &output)
{
    assert(rowCount + rowOffset <= this->rowCount);
    assert(columnCount + columnOffset <= this->columnCount);
    assert(rowCount == output.rowCount);
    assert(columnCount == output.columnCount);
    
    for (size_t i = 0, j = rowOffset; i < rowCount; ++i, ++j) {
        
        
        memcpy((T *)&output.data[i * columnCount], &this->data[j * stride + columnOffset], sizeof(T) * columnCount);
    }
    
}


template class Signals::Matrix<double>;
template class Signals::Matrix<float>;
template class Signals::StridedMatrix<double>;
template class Signals::StridedMatrix<float>;
