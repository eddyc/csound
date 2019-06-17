//
//  UKFFormant.cpp
//  Signals
//
//  Created by Edward Costello on 24/01/2018.
//  Copyright © 2018 Edward Costello. All rights reserved.
//

#include "ResampleOpcodes.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utilities.hpp"

using namespace std;
void copyDoubleToFloat(double *input, float *output, int length)
{

    for (int i = 0; i < length; ++i)
    {

        output[i] = input[i];
    }
}

void copyFloatToDouble(float *input, double *output, int length)
{

    for (int i = 0; i < length; ++i)
    {

        output[i] = input[i];
    }
}
void printArray(int length, float *data)
{

    for (int i = 0; i < length; ++i)
    {

        printf("%f ", data[i]);
    }
}

int DownsampleOpcode::init()
{
    srcState = src_new(0, 1, &error);
    cs = (CSOUND *)csound;
    inputArgumentCount = cs->GetInputArgCnt(this);
    outputArgumentCount = cs->GetOutputArgCnt(this);

    const int ksmps = cs->GetKsmps(cs);
    srcData.data_in = (float *)cs->Calloc(cs, sizeof(float) * ksmps);
    srcData.data_out = (float *)cs->Calloc(cs, sizeof(float) * ksmps);
    srcData.input_frames = ksmps;
    srcData.output_frames = ksmps;

    int blockSize = (int)*(double *)inargs(1);
    double ratio = (double)*inargs(2);
    srcData.src_ratio = ratio;

    input = (ARRAYDAT *)inargs(0);
    output = (ARRAYDAT *)outargs(0);
    size_t dimensions[1] = {(size_t)(input->sizes[0])};
    allocateArray(csound, dimensions, 1, output);

    int processed = 0;
    for (int i = 0; i < dimensions[0]; i += blockSize)
    {

        copyDoubleToFloat(&input->data[i], (float *)srcData.data_in, blockSize);
        // int error = src_process(srcState, &srcData);
        // copyFloatToDouble(srcData.data_out, (double *)&output->data[processed], srcData.output_frames_gen);
        // printArray(srcData.output_frames_gen, srcData.data_out);
        // processed += srcData.output_frames_gen;
    }
    hsize_t dimensions2[2] = {0};
    dimensions2[0] = 1;
    dimensions2[1] = processed;
    saveData("/Users/eddyc/Desktop/joe8k.h5", "/joe8k", dimensions2, output->data);

    return OK;
}

int DownsampleOpcode::kperf()
{
    // const int ksmps = cs->GetKsmps(cs);

    return OK;
}
