//
//  ScaledSymmetricSigmaPoints.cpp
//  Csound
//
//  Created by Edward Costello on 21/03/2018.
//

#include "ScaledSymmetricSigmaPoints.hpp"
#include <math.h>
auto allocateVector = [](size_t elementCount) -> tuple<double *, size_t> {
    return {
        (double *)calloc(elementCount, sizeof(double)),
        elementCount};
};

auto allocateMatrix = [](size_t rowCount, size_t columnCount) -> tuple<double *, size_t, size_t> {
    return {
        (double *)calloc(rowCount * columnCount, sizeof(double)),
        rowCount,
        columnCount};
};

ScaledSymmetricSigmaPoints::ScaledSymmetricSigmaPoints(size_t p) : p(allocateMatrix(p * 2 + 1, p * 2 + 1)),
                                                                   p2(allocateMatrix(p * 2 + 1, p * 2 + 1)),
                                                                   xPts(allocateMatrix(p * 2 + 1, (p * 2 + 1) * 2 + 1)),
                                                                   wPts(allocateVector((p + 1) * 4))
{
}

tuple<Matrix<double> &, Vector<double> &, size_t &> ScaledSymmetricSigmaPoints::operator()(const Vector<double> &x, const Matrix<double> &pIn, const double alpha, const double beta, double kappa)
{

    Matrix<double>::copy(pIn, p);
    double n = x.elementCount;
    nsp = 2 * n + 1;
    p.multiply(n + kappa);

    Matrix<double>::copy(p, p2);
    

    p.choleskyFactorisation();
    

    auto subMatA = xPts.subMatrix(p.rowCount, 0, p.columnCount, p.columnCount + 1);
    Matrix<double>::copy(p, subMatA);
    p.multiply(-1);

    auto subMatB = xPts.subMatrix(p.rowCount, 0, p.columnCount, 1);
    Matrix<double>::copy(p, subMatB);

    for (size_t i = 0; i < xPts.rowCount; ++i)
    {

        double &s = ((Vector<double> &)x)[i];
        xPts[i][0] = 0;
        xPts[i].add(s);
    }

    wPts.fill(0.5);
    wPts[0] = kappa;
    wPts.multiply(1 / (n + kappa));
    wPts.last() = wPts[0] + (1 - pow(alpha, 2)) + beta;
    //
    return forward_as_tuple(this->xPts, this->wPts, this->nsp);
}
