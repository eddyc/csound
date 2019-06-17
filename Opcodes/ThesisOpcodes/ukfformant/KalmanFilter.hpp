//
//  KalmanFilter.hpp
//  CsoundLib64
//
//  Created by Edward Costello on 23/08/2018.
//

#ifndef KalmanFilter_hpp
#define KalmanFilter_hpp

#include <stdio.h>

//
//  UKFFormant.hpp
//  KalmanFormantOpcode
//
//  Created by Edward Costello on 27/08/2017.
//  Copyright © 2017 Edward Costello. All rights reserved.
//

#include <plugin.h>
#include "../common/SignalData/Vector.hpp"
#include "../common/SignalData/Matrix.hpp"
#include "../common/FileIO.hpp"
#include "../common/json.hpp"
#include <samplerate.h>
#include "UKFRigollRecursion.hpp"
#include "ScaledSymmetricSigmaPoints.hpp"
#include "RingBuffer.hpp"

using namespace Signals;

typedef Matrix<double> Mat;
typedef Matrix<float> Matf;
typedef Vector<double> Vec;
typedef Vector<float> Vecf;

class KalmanFilter
{
    
    Vec xhat;
    
    double p;
    double alpha;
    double beta;
    double kappa;
    double vNoise;
    double wNoise;
    double noises;
    
    ScaledSymmetricSigmaPoints scaledSymmetricSigmaPts;
    double M;
    
    UKFRigollRecursion ukfRigollRecursion;
    
    Mat Qw;
    Mat Pcov;
    Mat PQ;
    Vec xQ;
    Mat wSigmaPts_xmat;
    Mat wSigmaPts_zmat;
    Mat xPredSigmaPts;
    Vec xEst;
    Vec vBuffer;
    Mat zPredSigmaPts;
    Mat xPred;
    Vec exSigmaPt;
    Mat exSigmaPtMat;
    Vec ezSigmaPtMat;
    Mat temp;
    Mat PPred;
    Vec PxzPred;
    Mat PxP;
    Mat PColumn;
    Mat K;
    size_t Fs;
public:
    KalmanFilter(CSOUND *csound, Vec &initFreqs, Vec &initBands, size_t ksmps, size_t fs);
    void process(Vec &input, double nextSample, Vec &formantVecOut);

};


#endif /* KalmanFilter_hpp */
