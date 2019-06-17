//
//  UKFFormant.cpp
//  Signals
//
//  Created by Edward Costello on 24/01/2018.
//  Copyright © 2018 Edward Costello. All rights reserved.
//

#include "ModVoc.hpp"
#include <stdlib.h>
int ModVoc::init() {

    cs = (CSOUND *)csound;
    ksmps = cs->GetKsmps(cs);
    plotter = Plotter<double>();
    new (&input) Vec(((MYFLT *)inargs.data(0)), ksmps);
    new (&output) Vec(((MYFLT *)outargs.data(0)), ksmps);
    windowSize = pow(2, 15);
    hopSize = windowSize / 8;

    function<MYFLT *(size_t)> allocator = [&](size_t count) -> MYFLT * {
        return (MYFLT *)(cs->Calloc(cs, sizeof(MYFLT) * count));
    };

    new (&window) Vec(allocator(windowSize), windowSize);
    new (&frameBuffer) FrameBuffer(ksmps, windowSize, hopSize, allocator);
    new (&dft) DFT<MYFLT>((size_t)windowSize, allocator);
    setWindow();
    return OK;
}

void ModVoc::setWindow() {
    window.sineWindow();
    plotter.x(window);
}

static MYFLT inMax = 0;
static MYFLT outMax = 0;

int ModVoc::kperf() {

    Vec in = Vec((MYFLT *)input.data, cs->GetKsmps(cs));
    Vec out = Vec((MYFLT *)output.data, cs->GetKsmps(cs));

    frameBuffer.process(in, out, [&](Vec inputFrame, Vec outputFrame) {
        Vec::copy(inputFrame, outputFrame);
    });

    for (int i = 0; i < in.elementCount; ++i) {

        if (in[i] > inMax) {

            inMax = in[i];
        }

        if (out[i] > outMax) {
            outMax = out[i];
        }
    }

    printf("%f %f\n", inMax, outMax);

    return OK;
}
