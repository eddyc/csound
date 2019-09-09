//
//  UKFFormant.cpp
//  Signals
//
//  Created by Edward Costello on 24/01/2018.
//  Copyright © 2018 Edward Costello. All rights reserved.
//

#include "FastConvolve.hpp"
#include "./common/Plot.hpp"
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
    allocator = [&](size_t count) -> MYFLT* {
        return (MYFLT*)(cs->Calloc(cs, sizeof(MYFLT) * count));
    };
    newVec = VectorFactory<MYFLT>(allocator);
    frameSize = 128;
    convSize = frameSize * 2;
    hopSize = frameSize / 2;
    new (&frameBuffer) FrameBuffer<MYFLT>(ksmps, hopSize, frameSize, allocator, true);
    new (&dft) DFT(allocator, (size_t)log2((float)frameSize));
    new (&zeropadDFT) DFT(allocator, (size_t)log2((float)convSize));
    new (&ain) Vec(((MYFLT*)inargs.data(0)), ksmps);
    new (&aout) Vec(((MYFLT*)outargs.data(0)), ksmps);
    new (&mags) Vec(allocator, frameSize / 2 + 1);
    new (&phases) Vec(allocator, frameSize / 2 + 1);
    new (&convBuffer) Vec(allocator, convSize);
    new (&convTail) Vec(allocator, frameSize);
    new (&window) Vec(allocator, frameSize);
    window.hanningWindow();
    STRINGDAT* filenameDat = (STRINGDAT*)(inargs.data(1));
    filename = filenameDat->data;
    new (&datfile) DATFile(allocator, filename);
    new (&timeDomainDAT) Mat(allocator, datfile.magnitudes.rowCount, convSize);

    for (int i = 0; i < datfile.magnitudes.rowCount; ++i) {
        auto mags = datfile.magnitudes[i];
        auto phases = datfile.phases[i];
        auto row = timeDomainDAT[i].sub(frameSize, 0);
        dft.polarToReal(mags, phases, row);
        // Plot<MYFLT>::x(row);
    }
    return OK;
}

int FastConvolve::kperf()
{
    frameBuffer.process(ain, aout, [&](const Vec& inputFrame, const Vec& outputFrame) -> void {
        convBuffer.shift(-frameSize);
        Vec::multiply(inputFrame, window, convBuffer.sub(frameSize, 0));
        // Plot<MYFLT>::x(convBuffer.sub(frameSize, 0));

        // Vec::multiply(inputFrame, window, inputFrame);
        // Plot<MYFLT>::x(inputFrame);
        // Vec::copy(inputFrame, convBuffer.sub(frameSize, 0));
        Vec::copy(convBuffer.sub(frameSize, 0), outputFrame);

        outputFrame.add(convTail);
        Vec::copy(convBuffer.sub(frameSize, frameSize), convTail);

        // Vec::copy(inputFrame, convBuffer.sub(frameSize, 0));

        // dft.realToPolar(inputFrame, mags, phases);
        // dft.polarToReal(mags, phases, outputFrame);
        // Vec::copy(convBuffer.sub(frameSize, 0), outputFrame);
        // outputFrame.add(convTail);
        // Vec::copy(convBuffer.sub(frameSize, frameSize), convTail);
        // Plot<MYFLT>::x(outputFrame);
    });

    // aout.print();
    return OK;
}
