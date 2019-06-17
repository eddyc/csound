//
//  ScaledSymmetricSigmaPoints.hpp
//  Csound
//
//  Created by Edward Costello on 21/03/2018.
//

#ifndef ScaledSymmetricSigmaPoints_hpp
#define ScaledSymmetricSigmaPoints_hpp
#include "../common/SignalData/Vector.hpp"
#include "../common/SignalData/Matrix.hpp"
using namespace Signals;
struct ScaledSymmetricSigmaPoints
{

    Matrix<double> p, p2, xPts;
    Vector<double> wPts;
    size_t nsp;

    ScaledSymmetricSigmaPoints(size_t p);

    tuple<Matrix<double> &, Vector<double> &, size_t &> operator()(const Vector<double> &x, const Matrix<double> &pIn,
                                                                   const double alpha, const double beta, double kappa);
};

#endif /* ScaledSymmetricSigmaPoints_hpp */
