//
//  UKFFormant.cpp
//  Signals
//
//  Created by Edward Costello on 24/01/2018.
//  Copyright © 2018 Edward Costello. All rights reserved.
//

#include "UKFFormant.hpp"
#include "utilities.hpp"

#define _write(input) writeFile(input, #input)
typedef int v4si __attribute__((__vector_size__(16)));

int UKFFormant::init()
{
    
    cs = (CSOUND *)csound;
    size_t ksmps = cs->GetKsmps(cs);
    
    v4si v = {1, 2, 3, 4};
    v = v + v;
    ARRAYDAT *initFreqArray = (ARRAYDAT *)inargs.data(1);
    ARRAYDAT *initBandArray = (ARRAYDAT *)inargs.data(2);
    const double downsamplerate = *inargs.data(3);
    
    if (initFreqArray->dimensions != initBandArray->dimensions ||
        initFreqArray->dimensions != 1 ||
        initFreqArray->sizes[0] != initBandArray->sizes[0])
    {
        
        csound->init_error("Error initialising opcode, initial frequencys/bands must be 1D arrays of equal size");
        return NOTOK;
    }
    
    new (&input) Vec((double *)inargs.data(0), ksmps);
    new (&inputf) Vecf((float *)calloc(ksmps, sizeof(float)), ksmps);
    new (&initFreqs) Vec(initFreqArray->data, initFreqArray->sizes[0]);
    new (&initBands) Vec(initBandArray->data, initBandArray->sizes[0]);
    
    
    double downRatio = downsamplerate/(double)cs->GetSr(cs);
    Fs = cs->GetSr(cs) * downRatio;
    
    p = 8;
    
    size_t resamplerInputChannels = 1;
    size_t resamplerOutputChannels = p + 1;
    
    new(&resamplerOut) Mat((double *)calloc(p, sizeof(double) * (p + 1) * ksmps), p + 1, ksmps);
    new(&formantBuffer) Vec((double *)calloc(p, sizeof(double) * p), p);

    new(&resampler) Resampler(cs, ksmps, p, resamplerInputChannels, resamplerOutputChannels, downRatio);
    
    new (&kalmanFilter) KalmanFilter(cs, initFreqs, initBands, cs->GetKsmps(cs), Fs);
    
    size_t dimensions[] = {initFreqs.elementCount, ksmps};
    allocateArray(csound, dimensions, 2, (ARRAYDAT *)outargs.data(0));
    allocateArray(csound, dimensions, 2, (ARRAYDAT *)outargs.data(1));
    new (&outFreqs) Mat(((ARRAYDAT *)outargs.data(0))->data, initFreqs.elementCount, ksmps);
    new (&outBands) Mat(((ARRAYDAT *)outargs.data(1))->data, initBands.elementCount, ksmps);
    new (&output) Vec(((MYFLT *)outargs.data(2)), ksmps);
    new (&pDoubleIn) Vec((double *)cs->Calloc(cs, sizeof(double) * p), p);
    new (&pDoubleOut) Vec((double *)cs->Calloc(cs, sizeof(double) * p), p);
    new (&inputResampledf) Vecf((float *)cs->Calloc(cs, sizeof(float) * p), p);
    new (&outputResampledf) Vecf((float *)cs->Calloc(cs, sizeof(float) * ksmps), ksmps);
    new (&deinterlacer) Vecf((float *)cs->Calloc(cs, sizeof(float) * ksmps * ksmps), ksmps * ksmps);

    new(&pBuffer) RingBuffer<double>(p * 2);
    
    pBuffer.write((double *)calloc(p, sizeof(double) * p), p);
    new(&pSamples) Vec((double *)calloc(p, sizeof(double) * p + 1), p + 1);
    new(&vBuffer) Vec((double *)calloc(p, sizeof(double) * p), p);
    
    new(&formantMatOut) Mat(allocateMatrix(p, p));
    new(&formantMatOutf) Matf((float *)calloc(p, sizeof(float) * p * p), p, p);

    return OK;
}

void writeFile(Matrix<double> input, string name)
{
    hsize_t dimensions[2] = {input.rowCount, input.columnCount};
    saveData("/Users/eddyc/Desktop/test.h5", "/" + name, dimensions, input.data);
}

void writeFile(Vector<double> input, string name)
{
    hsize_t dimensions[2] = {1, input.elementCount};
    saveData("/Users/eddyc/Desktop/test.h5", "/" + name, dimensions, input.data);
}

static MYFLT filterInputVector[2] = {0};
static MYFLT filterFeedbackVector[2] = {0};

void filter(size_t inNumberFrames, MYFLT *input)
{
    MYFLT bWeights[2] = {1, -0.98};
    MYFLT aWeight = 1;
    
    MYFLT feedforward, feedback;
    
    for (size_t i = 0; i < inNumberFrames; ++i)
    {
        
        feedback = feedforward = 0;
        filterInputVector[0] = input[i];
        
        for (int j = 0; j < 2; j++)
        {
            
            feedforward += bWeights[j] * filterInputVector[j];
            feedback += aWeight * filterFeedbackVector[j];
        }
        
        input[i] = feedforward - feedback;
        
        filterFeedbackVector[1] = filterFeedbackVector[0];
        filterInputVector[1] = filterInputVector[0];
    }
}

void copyVectorToFloat(Vec input, float *output) {
    
    for (int i = 0; i < input.elementCount; ++i) {
        output[i] = input.data[i];
    }
}

void copyFloatToVector(float *input , Vec output) {
    
    for (int i = 0; i < output.elementCount; ++i) {
        double *data = (double *)output.data;
        data[i]= input[i];
    }
}


void UKFFormant::stridedCopy(double *input, int length, int stride, int offset, double *output) {
    
    for (int i = 0; i < length; ++i) {
        
        output[i * stride + offset] = input[i];
    }
}


int UKFFormant::kperf() {
    
    Mat in = Matrix<MYFLT>((MYFLT *)input.data, 1, cs->GetKsmps(cs));
    Mat out = Matrix<MYFLT>((MYFLT *)output.data, 1, cs->GetKsmps(cs));
    
    resampler.process(in, resamplerOut, [&](Mat &reInput, Mat &reOutput) {
       
        filter(reInput[0].elementCount, (MYFLT *)reInput[0].data);
        pBuffer.write(reInput[0].data, p);
        
        for (int i = 0; i < p; ++i) {
            
            pBuffer.peak((double *)pSamples.data, p + 1);
            Vec pSamplesIn = pSamples.subVector(0, p);
            Vec::copy(pSamplesIn, vBuffer);
            double nextSample = pSamples[p];
            kalmanFilter.process(vBuffer, nextSample, formantBuffer);
          
            stridedCopy((double *)formantBuffer.data, p, p, i, (double *)reOutput.data);
            pBuffer.consume(1);
        }
        
//        reInput.print();
        Vec::copy(reInput[0], reOutput[reOutput.rowCount - 1]);
    });
    


    for (int i = 0; i < p; ++i) {
        
        if (i < 4) {
            
            Vec freqs = outFreqs[i];
            Vec resultFreqs = resamplerOut[i];
            Vec::copy(resultFreqs, freqs);
        }
        else {
            
            Vec bands = outBands[i % 4];
            Vec resultBands = resamplerOut[i];
            Vec::copy(resultBands, bands);
        }
    }
    
    Vec::copy(resamplerOut[resamplerOut.rowCount - 1], out);
    return OK;
}

