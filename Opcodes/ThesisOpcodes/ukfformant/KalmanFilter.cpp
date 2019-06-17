//
//  KalmanFilter.cpp
//  CsoundLib64
//
//  Created by Edward Costello on 23/08/2018.
//

#include "KalmanFilter.hpp"
#include "utilities.hpp"

KalmanFilter::KalmanFilter(CSOUND *csound, Vec &initFreqs, Vec &initBands,
                           size_t ksmps, size_t fs)
    : scaledSymmetricSigmaPts(8), ukfRigollRecursion(4), Fs(fs) {
    new (&xhat)
        Vec(allocateVector(initFreqs.elementCount + initBands.elementCount));
    auto xhatSubA = xhat.subVector(0, initFreqs.elementCount);
    auto xhatSubB =
        xhat.subVector(initFreqs.elementCount, initBands.elementCount);
    Vec::copy(initFreqs, xhatSubA);
    Vec::copy(initBands, xhatSubB);

    p = xhat.elementCount;

    alpha = 1;
    beta = 0;
    kappa = 2;
    vNoise = p;
    wNoise = 1;
    noises = vNoise + wNoise;

    new (&scaledSymmetricSigmaPts) ScaledSymmetricSigmaPoints(p);

    M = p / 2;

    new (&ukfRigollRecursion) UKFRigollRecursion(M);

    double observations = 1;
    new (&Qw) Mat(allocateMatrix(p, p));
    Qw.identity().multiply(50);

    new (&Pcov) Mat(allocateMatrix(p, p));
    new (&PQ) Mat(allocateMatrix(p * 2 + 1, p * 2 + 1));
    new (&xQ) Vec(allocateVector(p + noises));
    new (&wSigmaPts_xmat) Mat(allocateMatrix(p, xQ.elementCount * 2));
    new (&wSigmaPts_zmat)
        Mat(allocateMatrix(observations, xQ.elementCount * 2));
    new (&xPredSigmaPts) Mat(allocateMatrix(p, xQ.elementCount * 2 + 1));
    new (&xEst) Vec(allocateVector(p));
    //    new (&v) Vec(allocateVector(p));
    new (&zPredSigmaPts) Mat(allocateMatrix(1, xQ.elementCount * 2 + 1));
    new (&xPred) Mat(allocateMatrix(p, 1));
    new (&exSigmaPt) Vec(allocateVector(p));
    new (&exSigmaPtMat) Mat(allocateMatrix(p, xQ.elementCount * 2));
    new (&ezSigmaPtMat) Vec(allocateVector(xQ.elementCount * 2));
    new (&temp) Mat(allocateMatrix(xQ.elementCount * 2, 1));
    new (&PPred) Mat(allocateMatrix(p, p));
    new (&PxzPred) Vec(allocateVector(p));
    new (&PxP) Mat(allocateMatrix(p, p));
    new (&PColumn) Mat(allocateMatrix(p, 1));
    new (&K) Mat(allocateMatrix(p, 1));

    Vec::copy(xhat, xEst);
    Pcov.diagonal().fill(100);
    PQ.diagonal().subVector(p, p).fill(50);
    PQ.last() = 0.1;
}

void KalmanFilter::process(Vec &v, double speech_k, Vec &output) {

    auto PQSub = PQ.subMatrix(p, 0, p, 0);
    auto xQSub = xQ.subVector(0, xEst.elementCount);
    Mat::copy(Pcov, PQSub);
    Vec::copy(xEst, xQSub);

    auto [xSigmaPts, wSigmaPts, nsp] =
        scaledSymmetricSigmaPts(xQ, PQ, alpha, beta, kappa);

    wSigmaPts_xmat.repeatRows(
        wSigmaPts.subVector(1, wSigmaPts.elementCount - 2));
    Vec::copy(wSigmaPts.subVector(1, wSigmaPts.elementCount - 2),
              wSigmaPts_zmat);
    Mat::add(xSigmaPts.subMatrix(p, 0), xSigmaPts.subMatrix(p, p),
             xPredSigmaPts);

    v.reverse().multiply(-1);

    zPredSigmaPts.map([&, &xSigmaPts = xSigmaPts](double element, size_t l) {
        auto ahat = ukfRigollRecursion(
            xPredSigmaPts.subMatrix(xPredSigmaPts.rowCount, 0, 1, l), Fs);

        return Vec::dotProduct(v, ahat) + xSigmaPts[xSigmaPts.rowCount - 1][l];
    });

    xPred.map([&](double element, size_t i) {
        return wSigmaPts_xmat[i].reduce([&](double acc, double next, size_t j) {
            acc += next * (xPredSigmaPts[i][j + 1] - xPredSigmaPts[i][0]);
            return acc;
        });
    });

    double zPred =
        wSigmaPts_zmat.reduce([&](double acc, double next, size_t i) {
            acc += next * (zPredSigmaPts[0][i + 1] - zPredSigmaPts[0][0]);
            return acc;
        });

    auto colxPred = xPredSigmaPts.column(0);
    xPred.add(colxPred);
    zPred += zPredSigmaPts[0][0];

    Vec::subtract(colxPred, xPred, exSigmaPt);
    double ezSigmaPt = zPredSigmaPts[0][0] - zPred;

    Mat::outerProduct(exSigmaPt, exSigmaPt, PPred);
    PPred.multiply(wSigmaPts.last());

    Vec::multiply(exSigmaPt, ezSigmaPt, PxzPred);
    PxzPred.multiply(wSigmaPts.last());

    double S = wSigmaPts.last() * ezSigmaPt * ezSigmaPt;
    xPred.map([&](double e, size_t i) {
        Vec subexSigmaPtMat = exSigmaPtMat[i];
        Vec subxPredSigmaPts =
            xPredSigmaPts[i].subVector(1, xPredSigmaPts[i].elementCount - 1);
        Vec::subtract(subxPredSigmaPts, e, subexSigmaPtMat);
        return e;
    });

    auto zPredSub = zPredSigmaPts.subVector(1, zPredSigmaPts.elementCount - 1);
    Vec::subtract(zPredSub, zPred, ezSigmaPtMat);
    Vec::multiply(wSigmaPts_xmat, exSigmaPtMat, wSigmaPts_xmat);
    Mat::multiplyTransposed(exSigmaPtMat, wSigmaPts_xmat, PxP);
    Vec::add(PPred, PxP, PPred);
    Vec::multiply(wSigmaPts_zmat, ezSigmaPtMat, temp);

    S += Vec::dotProduct(temp, ezSigmaPtMat);

    Mat::multiply(exSigmaPtMat, temp, PColumn);
    Vec::add(PxzPred, PColumn, PxzPred);
    Vec::divide(PxzPred, S, K);

    double innovation = speech_k - zPred;
    Vec::copy(xEst, output);

    Vec::multiply(K, innovation, xEst);
    Vec::add(xEst, xPred, xEst);

    Mat::multiplyTransposed(K, K, PxP);
    PxP.multiply(S);
    Vec::subtract(PPred, PxP, Pcov);
}
