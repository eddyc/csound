//
//  MatrixMaths.hpp
//  Signals
//
//  Created by Edward Costello on 15/01/2018.
//  Copyright © 2018 Edward Costello. All rights reserved.
//

#ifndef MatrixMaths_hpp
#define MatrixMaths_hpp

Matrix<T> &identity();
Matrix<T> &choleskyFactorisation();
Matrix<T> &choleskyFactorisation2();
Matrix<T> &multiply(T scalar);

//static void multiply(const Matrix<T> &inputA, const Matrix<T> &inputB, Matrix<T> &output);
static void multiply(const Matrix<double> &inputA, const Matrix<double> &inputB, Matrix<double> &output);
static void multiply(const Matrix<double> &inputA, bool, const Matrix<double> &inputB, bool, Matrix<double> &output);
static void multiplyTransposed(const Matrix<double> &inputA,
                               const Matrix<double> &inputB,
                               Matrix<double> &output);

bool equals(const Matrix<T> other);
static void transpose(const Matrix<double> &inputA, const Matrix<double> &inputB, Matrix<double> &output);

#endif /* MatrixMaths_hpp */
