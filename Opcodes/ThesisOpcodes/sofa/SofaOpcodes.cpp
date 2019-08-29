//
//  UKFFormant.cpp
//  Signals
//
//  Created by Edward Costello on 24/01/2018.
//  Copyright © 2018 Edward Costello. All rights reserved.
//

#include "SofaOpcodes.hpp"
#include "utilities.hpp"
#include <netcdf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int frameSize = 128;
const int hopSize = frameSize / 4;

class SOFAFile {
public:
    int dimensionCount;
    int variableCount;
    int attributeCount;
    int unlimitedDimensionsID;
};

using namespace std;

void printArray(int length, float* data)
{
    for (int i = 0; i < length; ++i) {
        printf("%f ", data[i]);
    }
}

int SofaOpcode::init()
{
    CSOUND* cs = (CSOUND*)csound;

    const int ksmps = cs->GetKsmps(cs);
    function<MYFLT*(size_t)> allocator = [&](size_t count) -> MYFLT* {
        return (MYFLT*)(cs->Calloc(cs, sizeof(MYFLT) * count));
    };
    new (&input) Vec(((MYFLT*)inargs.data(0)), ksmps);
    new (&output) Vec(((MYFLT*)outargs.data(0)), ksmps);
    new (&window) Vec(allocator(frameSize), frameSize);
    // string filenameL =
    //     "/Users/eddyc/Documents/Software/HRTF
    //     Project/Mesh2HRTF/Meshes/KemarL/" "EvaluationGrid.sofa";
    // string filenameR =
    //     "/Users/eddyc/Documents/Software/HRTF
    //     Project/Mesh2HRTF/Meshes/KemarR/" "EvaluationGrid.sofa";

    // fileL = NetCDFFile(filenameL, "TF");
    // fileR = NetCDFFile(filenameR, "TF");

    new (&frameBuffer) FrameBuffer<MYFLT>(ksmps, frameSize, hopSize, allocator);
    cs->Warning(cs, "warning here\n");
    return OK;
}

int SofaOpcode::kperf()
{
    CSOUND* cs = (CSOUND*)csound;
    // const int ksmps = cs->GetKsmps(cs);
    Vec in = Vec((MYFLT*)input.data, cs->GetKsmps(cs));
    Vec out = Vec((MYFLT*)output.data, cs->GetKsmps(cs));

    cout << "hello" << endl;
    frameBuffer.process(in, out, [&](Vec inputFrame, Vec outputFrame) {
        Vec::copy(inputFrame, outputFrame);
    });
    cs->Warning(cs, "warning here\n");

    sleep(23);
    return OK;
}
