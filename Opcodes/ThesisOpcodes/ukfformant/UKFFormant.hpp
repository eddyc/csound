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
#include "KalmanFilter.hpp"
#include "Resampler.hpp"

using namespace Signals;

typedef Matrix<double> Mat;
typedef Matrix<float> Matf;
typedef Vector<double> Vec;
typedef Vector<float> Vecf;

class UKFFormant : public csnd::Plugin<3, 4>
{
    Resampler resampler;
    KalmanFilter kalmanFilter;

    Vec input, output, buffer, initFreqs, initBands, filterTest, pDoubleIn, pDoubleOut, pSamples;
    Vecf deinterlacer;
    Vecf inputf, inputResampledf;
    Vecf outputf, outputResampledf;
    Mat outFreqs, outBands;
    size_t Fs;
    CSOUND *cs;
    Vec vBuffer;
    int p;
    Mat formantMatOut;
    Matf formantMatOutf;
    
    Mat resamplerOut;
    Vec formantBuffer;
    int errorUp, errorDown;
    RingBuffer<double> pBuffer;

  public:
    int init();
    static int init_(CSOUND *csound, UKFFormant *self)
    {
        self->init();
        return OK;
    }
    int kperf();
    int _kperf();
    static int kperf_(CSOUND *csound, UKFFormant *self)
    {
        self->kperf();
        return OK;
    }
    
    void process(Vec &input, double nextSample, Vec &formantVecOut);
    void downSample(Vec &input, Vec &output, Mat &freqsOutput, Mat &bandsOutput, function<void(Vec &samplesIn, Vec &samplesOut, Mat &formantsOut)> process);
    void stridedCopy(double *input, int length, int stride, int offset, double *output);


};
