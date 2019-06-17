//
//  Matrix.hpp
//  Signals
//
//  Created by Edward Costello on 15/01/2018.
//  Copyright © 2018 Edward Costello. All rights reserved.
//

#ifndef Matrix_hpp
#define Matrix_hpp

#include "Vector.hpp"

namespace Signals {
    

    template <typename T>
    class StridedMatrix;
    
    template <typename T>
    class Matrix : public Vector<T> {
        
    public:
        
        const size_t rowCount;
        const size_t columnCount;
        Matrix<T>();
        Matrix<T>(tuple<T *, size_t, size_t>);
        Matrix<T>(T *, size_t, size_t);
        Vector<T> operator[](size_t index) ;
        static void copy( Matrix<T> input, StridedMatrix<T> output);
        Matrix<T> &repeatRows(const Vector<T> input);
        StridedMatrix<T> subMatrix(size_t rowCount, size_t rowOffset, size_t columnCount, size_t columnOffset);
        Matrix<T> subMatrix(size_t rowCount, size_t rowOffset);
        StridedVector<T> diagonal();
        StridedVector<T> column(size_t index);
        void print();
#include "MatrixMaths.hpp"
    };
    
    template<typename T>
    class StridedMatrix : Vector<T> {
        
    public:
        const size_t rowCount;
        const size_t columnCount;
        const size_t stride;
        StridedMatrix();
        StridedMatrix<T>(Matrix<T>);
        StridedMatrix<T>(Matrix<T> input, size_t rowCount, size_t rowOffset,
                         size_t columnCount, size_t columnOffset);
        Vector<T> operator[](size_t index) const;
       
        void copy(size_t rowCount, size_t rowOffset, size_t columnCount, size_t columnOffset, Matrix<T> &output);
        void print();
        
    };
    
}
#endif /* Matrix_hpp */
