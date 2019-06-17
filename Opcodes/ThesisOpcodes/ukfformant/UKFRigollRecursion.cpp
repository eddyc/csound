//
//  UKFRigollRecursion.cpp
//  CsoundLib64
//
//  Created by Edward Costello on 21/03/2018.
//

#include "UKFRigollRecursion.hpp"
#include <math.h>

auto allocateMatrix = [](size_t rowCount, size_t columnCount) -> tuple<double *, size_t, size_t> {
    return {
        (double *)calloc(rowCount * columnCount, sizeof(double)),
        rowCount,
        columnCount};
};

UKFRigollRecursion::UKFRigollRecursion(size_t M) : ahat(allocateMatrix(2 * M, 1)),
                                                   ahatBuffer(allocateMatrix(2 * M, 1)),
                                                   A(allocateMatrix(2 * M, 2 * M)),
                                                   b(allocateMatrix(2 * M, M)),
                                                   c(allocateMatrix(M, 1)),
                                                   d(allocateMatrix(M, 1)),
                                                   M(M)
{
}

Matrix<double> &UKFRigollRecursion::operator()(StridedMatrix<double> xPredSigmaPts, size_t Fs)
{
    ahat.fill(0);
    xPredSigmaPts.copy(M, M, 1, 0, c);
    xPredSigmaPts.copy(M, 0, 1, 0, d);
    c.multiply(-M_PI / (double)Fs)
        .exponent()
        .multiply(-2);

    d.multiply(2 * M_PI / (double)Fs)
        .cosine();
    Vector<double>::multiply(c, d, c);

    xPredSigmaPts.copy(M, M, 1, 0, d);
    d.multiply(-2 * M_PI / (double)Fs)
        .exponent();

    ahat[0][0] = b[0][0] = c[0][0];
    ahat[1][0] = b[1][0] = d[0][0];

    for (size_t i = 1; i < M; ++i)
    {

        A.identity();
        A[1][0] = c[i][0];
        A[2 * M - 1][2 * M - 2] = c[i][0];
        A[2 * M - 1][2 * M - 3] = d[i][0];

        for (size_t i1 = 2; i1 < (2 * M) - 1; ++i1)
        {

            A[i1][i1 - 1] = c[i][0];
            A[i1][i1 - 2] = d[i][0];
        }

        b[0][i] = c[i][0];
        b[1][i] = d[i][0];

        Matrix<double>::multiply(A, ahat, ahatBuffer);

        for (size_t j = 0; j < b.rowCount; ++j)
        {

            ahat[j][0] = ahatBuffer[j][0] + b[j][i];
        }
    }

    return ahat;
}
