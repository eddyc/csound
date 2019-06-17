//
//  UKFRigollRecursion.hpp
//  CsoundLib64
//
//  Created by Edward Costello on 21/03/2018.
//

#ifndef UKFRigollRecursion_hpp
#define UKFRigollRecursion_hpp
#include "../common/SignalData/Matrix.hpp"
#include "../common/SignalData/Vector.hpp"

using namespace Signals;
struct UKFRigollRecursion
{
    Matrix<double> ahat, ahatBuffer, A, b, c, d;
    size_t M;
    UKFRigollRecursion(size_t M);
    Matrix<double> &operator()(StridedMatrix<double> xPredSigmaPts, size_t Fs);
};

#endif /* UKFRigollRecursion_hpp */
