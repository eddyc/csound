//
//  UKFFormant.cpp
//  Signals
//
//  Created by Edward Costello on 24/01/2018.
//  Copyright © 2018 Edward Costello. All rights reserved.
//

#include "ModVoc.hpp"
#include <stdlib.h>
int ModVoc::init()
{
    CSOUND* cs = (CSOUND*)csound;

    const int ksmps = cs->GetKsmps(cs);
    allocator = [&](size_t count) -> MYFLT* {
        return (MYFLT*)(cs->Calloc(cs, sizeof(MYFLT) * count));
    };
    new (&ain) Vec(((MYFLT*)inargs.data(0)), ksmps);
    new (&aout) Vec(((MYFLT*)outargs.data(0)), ksmps);

    hopSize = 256;
    frameSize = 1024;

    new (&frameBuffer) FrameBuffer<MYFLT>(ksmps, hopSize, frameSize, allocator, false);
    return OK;
}

void ModVoc::setWindow()
{
}

int ModVoc::kperf()
{
    frameBuffer.process(ain, aout, [&](Vec input, Vec output) -> void {
        Vec::copy(input, output);
    });
    return OK;
}
