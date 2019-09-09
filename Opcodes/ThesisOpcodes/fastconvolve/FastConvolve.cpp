//
//  UKFFormant.cpp
//  Signals
//
//  Created by Edward Costello on 24/01/2018.
//  Copyright © 2018 Edward Costello. All rights reserved.
//

#include "FastConvolve.hpp"
#include <fstream>
#include <netcdf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

int FastConvolve::init()
{
    CSOUND* cs = (CSOUND*)csound;

    const int ksmps = cs->GetKsmps(cs);
    const auto allocator = [&](size_t count) -> MYFLT* {
        return (MYFLT*)(cs->Calloc(cs, sizeof(MYFLT) * count));
    };
    newVec = VectorFactory<MYFLT>(allocator);

    const size_t frameSize = 64;
    const size_t hopSize = 32;
    new (&frameBuffer) FrameBuffer<MYFLT>(ksmps, hopSize, frameSize, allocator, false);
    const auto data1 = newVec(10).ramp(0, 9);
    const auto data2 = newVec(2).ramp(0, 1);
    data1.push(data2);
    data1.print();
    data1.shift(2);
    data1.print();
    return OK;
}

int FastConvolve::kperf()
{
    CSOUND* cs = (CSOUND*)csound;

    const int ksmps = cs->GetKsmps(cs);

    new (&ain) Vec(((MYFLT*)inargs.data(0)), ksmps);
    new (&aout) Vec(((MYFLT*)outargs.data(0)), ksmps);

    frameBuffer.process(ain, aout, [&](const Vec& inputFrame, const Vec& outputFrame) -> void {
        Vec::copy(inputFrame, outputFrame);
    });

    // aout.print();
    return OK;
}
