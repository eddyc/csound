//
//  Signal.cpp
//  UtilityClasses
//
//  Created by Edward Costello on 14/09/2017.
//  Copyright © 2017 Edward Costello. All rights reserved.
//

#include "Signal.hpp"
#ifdef __APPLE__
#include <Accelerate/Accelerate.h>
#endif

template <typename T>
Vector<T>::Iterator::Iterator(const Vector<T> *vector, size_t position)
    : position(position), vector(vector) {}

template <typename T>
bool Vector<T>::Iterator::operator!=(const Iterator &other) const {
    return (position != other.position);
}

template <typename T> T &Vector<T>::Iterator::operator*() const {
    return (*vector)[position];
}
template <typename T>
const typename Vector<T>::Iterator &Vector<T>::Iterator::operator++() {
    position++;
    return *this;
}

template <typename T>
Vector<T>::Vector(T *data, size_t elementCount)
    :
data(data),
elementCount(elementCount)
{}

template <typename T>
Vector<T>::Vector(size_t elementCount,
                  std::function<T *(size_t input)> memoryAllocator)
    : elementCount(elementCount), data(memoryAllocator(elementCount)) {}

template <typename T>
hsize_t Vector<T>::getH5ElementCount(std::string path, std::string dataSet) {
    herr_t status;
    hid_t fileID, dataSetID, dataSpaceID;
    hsize_t dimensions[2] = {0};

    fileID = H5Fopen(path.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);

    dataSetID = H5Dopen2(fileID, dataSet.c_str(), H5P_DEFAULT);
    dataSpaceID = H5Dget_space(dataSetID);

    H5Sget_simple_extent_dims(dataSpaceID, dimensions, NULL);

    status = H5Sclose(dataSpaceID);
    status = H5Dclose(dataSetID);
    status = H5Fclose(fileID);

    return dimensions[0] * dimensions[1];
}

template <typename T>
Vector<T>::Vector(std::string path, std::string dataset,
                  std::function<T *(size_t input)> memoryAllocator)
    : elementCount(getH5ElementCount(path, dataset)),
      data(memoryAllocator(getH5ElementCount(path, dataset))) {
    hid_t fileID, dataSetID, dataSpaceID;
    hsize_t dimensions[2] = {0};

    herr_t status;

    fileID = H5Fopen(path.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);

    dataSetID = H5Dopen2(fileID, dataset.c_str(), H5P_DEFAULT);
    dataSpaceID = H5Dget_space(dataSetID);

    H5Sget_simple_extent_dims(dataSpaceID, dimensions, NULL);

    status = H5Dread(dataSetID, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL,
                     H5P_DEFAULT, (void *)data);

    status = H5Sclose(dataSpaceID);
    status = H5Dclose(dataSetID);
    status = H5Fclose(fileID);
}

template <typename T>
Vector<T>::Vector(Vector<T> &input, size_t offset, size_t elementCount,
                  size_t stride)
    : elementCount(elementCount), data(&input.data[offset]),
      stride(input.stride * stride) {
    assert(stride != 0);
    assert(offset + (elementCount - 1) * stride + 1 <= input.elementCount);
}

template <typename T>
Vector<T> Vector<T>::subVector(size_t offset, size_t elementCount,
                               size_t stride) {
    return Vector<T>(*this, offset, elementCount, stride);
}

template <typename T> typename Vector<T>::Iterator Vector<T>::begin() const {
    return Iterator(this, 0);
}

template <typename T> typename Vector<T>::Iterator Vector<T>::end() const {
    return Iterator(this, elementCount);
}

template <typename T> T &Vector<T>::operator[](size_t n) const {

    assert(n < elementCount);
    return ((T *)data)[n * stride];
}

template <typename T> void Vector<T>::print() {
    cout << "Vector::print:" << endl;

    for (size_t i = 0; i < elementCount; ++i) {

        cout << "[" << ((Vector) * this)[i] << "]";
    }

    cout << "\n" << endl;
}

template <typename T>
void Vector<T>::copyData(Vector<T> &source, Vector<T> &destination) {
    assert(source.elementCount == destination.elementCount);

    memcpy(&destination.data, source.data, sizeof(T) * source.elementCount);
}

template <> Vector<double> &Vector<double>::fill(const double value) {
#ifdef __APPLE__
    vDSP_vfillD(&value, (double *)data, stride, elementCount);
#else
    for (auto &i : *this) {

        i = value;
    }
#endif
    return *this;
}

template <typename T> Vector<T> &Vector<T>::fill(T value) {
    for (auto &i : *this) {

        i = value;
    }
    return *this;
}

template <> Vector<double> &Vector<double>::multiply(const double value) {
#ifdef __APPLE__
    vDSP_vsmulD((double *)data, stride, &value, (double *)data, stride,
                elementCount);
#else
    for (auto &i : *this) {

        i *= value;
    }
#endif
    return *this;
}

template <typename T> Vector<T> &Vector<T>::multiply(T value) {
    for (auto &i : *this) {

        i = value;
    }
    return *this;
}

template <typename T>
Matrix<T>::Iterator::Iterator(const Matrix<T> *matrix, size_t position)
    : position(position), matrix(matrix) {}

template <typename T>
bool Matrix<T>::Iterator::operator!=(const Iterator &other) const {
    return (position != other.position);
}

template <typename T> Vector<T> Matrix<T>::Iterator::operator*() const {
    return (*matrix)[position];
}

template <typename T>
const typename Matrix<T>::Iterator &Matrix<T>::Iterator::operator++() {
    position++;
    return *this;
}

template <typename T>
Matrix<T>::Matrix(T *data, size_t rowCount, size_t columnCount)
    : Vector<T>(data, rowCount * columnCount), rowCount(rowCount),
      columnCount(columnCount) {}

template <typename T>
Matrix<T>::Matrix(T *data, size_t elementCount)
    : Matrix<T>(data, elementCount, elementCount) {}

template <typename T> typename Matrix<T>::Iterator Matrix<T>::begin() const {
    return Iterator(this, 0);
}

template <typename T> typename Matrix<T>::Iterator Matrix<T>::end() const {

    return Iterator(this, rowCount);
}

template <typename T> Vector<T> Matrix<T>::operator[](size_t n) const {

    assert(n < rowCount);
    return Vector<T>(&(T &)Vector<T>::data[n * columnCount], columnCount);
}

template <typename T> void Matrix<T>::print() const {
    cout << "Matrix::print:" << endl;

    for (size_t i = 0; i < rowCount; ++i) {

        for (size_t j = 0; j < columnCount; ++j) {

            cout << "[" << (*this)[i][j] << "]";
        }

        cout << endl;
    }

    cout << endl;
}

template <typename T> Vector<T> Matrix<T>::diagonal() {
    assert(rowCount == columnCount);
    return this->subVector(0, rowCount, rowCount + 1);
}

template <typename T> Matrix<T> &Matrix<T>::identity() {
    assert(rowCount == columnCount);
    this->diagonal().fill(1);

    return *this;
}

template <typename T> Matrix<T> &Matrix<T>::multiply(T value) {
    Vector<T>::multiply(value);
    return *this;
}

template <typename T> void Matrix<T>::operator*=(T value) {
    Vector<T>::multiply(value);
}

template class Vector<double>;
// template class Vector<float>;
// template class Vector<int>;

template class Matrix<double>;
// template class Matrix<float>;
// template class Matrix<int>;
