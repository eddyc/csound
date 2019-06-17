//
//  VectorMaths.hpp
//  Signals
//
//  Created by Edward Costello on 15/01/2018.
//  Copyright © 2018 Edward Costello. All rights reserved.
//

#ifndef VectorMaths_hpp
#define VectorMaths_hpp
// typedef double vec4 __attribute__((ext_vector_type(4)));

Vector<T> &fill(const T);
Vector<T> &clear();
Vector<T> &multiply(const T);
Vector<T> &add(const T);
Vector<T> &add(StridedVector<T> &);
Vector<T> &exponent();
Vector<T> &cosine();
Vector<T> &reverse();
Vector<T> &sineWindow();
static void add(const Vector<T> &inputA, const Vector<T> &inputB,
                Vector<T> &output);
static void divide(const Vector<T> &inputA, const T &scalar, Vector<T> &output);
Vector<T> &divide(T scalar);

static void multiply(const Vector<T> &input, const T &scalar,
                     Vector<T> &output);
static void multiply(const Vector<T> &inputA, const Vector<T> &inputB,
                     Vector<T> &output);
static void subtract(const StridedVector<T> &inputA, const Vector<T> &inputB,
                     Vector<T> &output);
static void subtract(Signals::Vector<T> &input, double scalar,
                     Signals::Vector<T> &output);
static void subtract(Vector<T> &inputA, Vector<T> &inputB, Vector<T> &output);

static T dotProduct(const Vector<T> &inputA, const Vector<T> &inputB);
static void outerProduct(const Vector<T> &inputA, const Vector<T> &inputB,
                         Matrix<T> &output);

Vector<T> &ramp(const T start, const T increment);

#endif /* VectorMaths_hpp */
