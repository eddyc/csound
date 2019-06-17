//
//  Signal.hpp
//  UtilityClasses
//
//  Created by Edward Costello on 14/09/2017.
//  Copyright © 2017 Edward Costello. All rights reserved.
//

#include "hdf5.h"
#include <assert.h>
#include <functional>
#include <iostream>
#include <stddef.h>

using namespace std;
template <typename T> class CsVector;
template <typename T> class Vector {

    friend CsVector<T>;

    class Iterator {

      public:
        Iterator(const Vector<T> *vector, size_t position);
        bool operator!=(const Iterator &other) const;
        T &operator*() const;
        const Iterator &operator++();

      private:
        size_t position;
        const Vector<T> *vector;
    };

  protected:
    T const *data;

  public:
    const size_t elementCount;
    const size_t stride = 1;

    Vector<T>(T *data, size_t elementCount);
    Vector<T>(size_t elementCount,
              std::function<T *(size_t input)> memoryAllocator);
    Vector<T>(Vector<T> &input, size_t offset, size_t elementCount,
              size_t stride);
    Vector<T>(std::string path, std::string dataset,
              std::function<T *(size_t input)> memoryAllocator);
    Vector<T> subVector(size_t offset, size_t elementCount, size_t stride);

    static void copyData(Vector<T> &source, Vector<T> &destination);
    static hsize_t getH5ElementCount(std::string path, std::string dataSet);

    Iterator begin() const;
    Iterator end() const;

    T &operator[](size_t n) const;

    void print();

    Vector<T> &fill(T value);
    Vector<T> &multiply(T value);
};

template <typename T> class Matrix : public Vector<T> {

    class Iterator {

      public:
        Iterator(const Matrix<T> *matrix, size_t position);
        bool operator!=(const Iterator &other) const;
        Vector<T> operator*() const;
        const Iterator &operator++();

      private:
        size_t position;
        const Matrix<T> *matrix;
    };

  public:
    const size_t rowCount;
    const size_t columnCount;

    Matrix(T *data, size_t rowCount, size_t columnCount);
    Matrix(T *data, size_t elementCount);
    Iterator begin() const;
    Iterator end() const;
    Vector<T> operator[](size_t n) const;
    void print() const;

    Vector<T> diagonal();
    Matrix<T> &identity();
    Matrix<T> &fill(T value);
    Matrix<T> &multiply(T value);
    void operator*=(T value);
};

template <typename T> class PVFrame {

    class Iterator {

      public:
        Iterator(const PVFrame<T> *frame, size_t position);
        bool operator!=(const Iterator &other) const;
        T &operator*() const;
        const Iterator &operator++();

      private:
        size_t position;
        const PVFrame<T> *frame;
    };

  protected:
    Vector<T> const real;
    Vector<T> const imag;
};
