//
//  Resampler.hpp
//  CsoundLib64
//
//  Created by Edward Costello on 23/08/2018.
//

#ifndef Resampler_hpp
#define Resampler_hpp

#include <stdio.h>
#include <plugin.h>
#include "../common/SignalData/Vector.hpp"
#include "../common/SignalData/Matrix.hpp"
#include <samplerate.h>
#include "RingBuffer.hpp"

using namespace Signals;

typedef Matrix<double> Mat;
typedef Matrix<float> Matf;
typedef Vector<double> Vec;
typedef Vector<float> Vecf;

class Resampler {
   
    int errorUp, errorDown;
    SRC_DATA upData, downData;
    SRC_STATE *upState, *downState;
    Vecf inputDeinterlace, outputDeinterlace, processInputf, processOutputf, outputf;
    Mat processInput, processOutput, bufferedInput, bufferedOutput;
    RingBuffer<float> *downRingBuffer, *upRingBuffer;
    RingBuffer<double> *inputRingBuffer, *outputRingBuffer;
public:
    
    Resampler(CSOUND *csound, int inputVectorSize, int processVectorSize,
              int inputChannelCount, int outputChannelCount, double ratio);
    void process(Mat &input, Mat &output, function<void(Mat &in, Mat &out)> resampledProcess);
    void resample(Mat &input, Mat &output, function<void(Mat &in, Mat &out)> resampledProcess);
    void copyVectorToFloat(Vec input, float *output);
    void copyVectorToFloat(Vec input, int stride, int offset, float *output);

    void copyFloatToVector(float *input , Vec output);
};

#endif /* Resampler_hpp */
