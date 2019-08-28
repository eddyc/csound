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
    string filenameL =
        "/Users/eddyc/Documents/Software/HRTF Project/Mesh2HRTF/Meshes/KemarL/"
        "EvaluationGrid.sofa";
    string filenameR =
        "/Users/eddyc/Documents/Software/HRTF Project/Mesh2HRTF/Meshes/KemarR/"
        "EvaluationGrid.sofa";

    fileL = NetCDFFile(filenameL, "TF");
    fileR = NetCDFFile(filenameR, "TF");
    CSOUND* cs = (CSOUND*)csound;
    const int ksmps = cs->GetKsmps(cs);
    function<MYFLT*(size_t)> allocator = [&](size_t count) -> MYFLT* {
        return (MYFLT*)(cs->Calloc(cs, sizeof(MYFLT) * count));
    };

    new (&frameBuffer) FrameBuffer<MYFLT>(ksmps, frameSize, hopSize, allocator);

    return OK;
}

int SofaOpcode::kperf()
{
    return OK;
}
