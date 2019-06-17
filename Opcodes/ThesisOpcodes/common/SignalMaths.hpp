//
//  MatrixMaths.hpp
//  DTW
//
//  Created by Edward Costello on 16/02/2017.
//  Copyright © 2017 Edward Costello. All rights reserved.
//


#ifndef _SignalMaths_
#define _SignalMaths_

#include "SignalData.hpp"

namespace SignalData {
    
    class SignalMaths {
    public:
        
        static void add(const Vector &input, const MYFLT scalar);
        static void add(const Vector &inputA, const Vector &inputB, const Vector& output);
        static void multiply(const Vector &inputA, const Vector &inputB, const Vector &output);
        static void multiply(const Matrix &inputA, const Matrix &inputB, const Matrix &output);
        static const Vector &multiply(const Vector &input, const MYFLT scalar);
        static void multiply(const Vector &input, const MYFLT scalar, Vector &output);
        static void divide(const Vector &input, const MYFLT scalar, Vector &output);
        static const Vector &divide(const Vector &input, const MYFLT scalar);
        static void substract(const Vector &inputA, const Vector &inputB, const Vector& output);
        static void substract(const Vector &inputA, const MYFLT inputB, const Vector& output);
        static void square(const Vector &input, const Vector &output);
        static void square(const Vector &input);
        static void exponent(const Vector &input);
        static void cosine(const Vector &input);
        static MYFLT sum(const Vector &input);
        static void fill(const Vector &input, MYFLT value);
        static void fillRow(const Matrix &input, size_t row, MYFLT value);
        static void fillColumn(const Matrix &input, size_t column, MYFLT value);
        static std::pair<size_t, MYFLT> minimum(Vector &input);
        static void reverse(const Vector &input);
        static MYFLT dotProduct(const Vector &inputA, const Vector &inputB);
        static void transpose(const Matrix &inputA, const Matrix &inputB);
        static void transpose(const PVSegment &inputA, const PVSegment &inputB);
        static void ramp(const Vector &input, const MYFLT initial, const MYFLT delta);
        static void linearInterpolateRows(Matrix &input, Matrix &output);
        static void linearInterpolateRows(PVSegment &input, PVSegment &output);
        static const Matrix &identity(const Matrix &input);
        static const Matrix &identity(const Matrix &input, MYFLT diagonal);
        static void choleskyFactorisation(const Matrix &input);
        static void sumRows(const Matrix &input, Vector &output);
        static const Vector &clear(const Vector &input);
    };
    
}
#endif
