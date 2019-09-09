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
    hopSize = frameSize;
    new (&frameBuffer) FrameBuffer<MYFLT>(ksmps, hopSize, frameSize, allocator, false);
    new (&dft) DFT(allocator, (size_t)log2((float)frameSize));
    new (&zeropadDFT) DFT(allocator, (size_t)log2((float)convSize));
    new (&ain) Vec(((MYFLT*)inargs.data(0)), ksmps);
    new (&aout) Vec(((MYFLT*)outargs.data(0)), ksmps);
    new (&mags) Vec(allocator, convSize / 2 + 1);
    new (&phases) Vec(allocator, convSize / 2 + 1);
    new (&convBuffer) Vec(allocator, convSize);
    new (&convTail) Vec(allocator, frameSize);
    new (&window) Vec(allocator, frameSize);
    window.hanningWindow();
    STRINGDAT* filenameDat = (STRINGDAT*)(inargs.data(1));
    filename = filenameDat->data;
    new (&datfile) DATFile(allocator, filename);
    new (&timeDomainDAT) Mat(allocator, datfile.magnitudes.rowCount, convSize);
    new (&fileMags) Mat(allocator, datfile.magnitudes.rowCount, convSize / 2 + 1);
    new (&filePhases) Mat(allocator, datfile.magnitudes.rowCount, convSize / 2 + 1);

    for (int i = 0; i < datfile.magnitudes.rowCount; ++i) {
        auto mags = datfile.magnitudes[i];
        auto phases = datfile.phases[i];
        auto row = timeDomainDAT[i].sub(frameSize, 0);
        dft.polarToReal(mags, phases, row);
        Vec::multiply(window, row, row);
        auto fullRow = timeDomainDAT[i];
        auto fileMagRow = fileMags[i];
        auto filePhaseRow = filePhases[i];
        zeropadDFT.realToPolar(fullRow, fileMagRow, filePhaseRow);
    }
    return OK;
}

int FastConvolve::kperf()
{
    int index = *((MYFLT*)inargs.data(2));
    auto fileMag = fileMags[index];
    auto filePhase = filePhases[index];
    frameBuffer.process(ain, aout, [&](const Vec& inputFrame, const Vec& outputFrame) -> void {
        Vec::copy(inputFrame, convBuffer.sub(frameSize, frameSize));
        Vec::copy(convTail, convBuffer.sub(frameSize, 0));
        zeropadDFT.realToPolar(convBuffer, mags, phases);

        Vec::copy(inputFrame, convTail);
        Vec::multiply(mags, fileMag, mags);
        Vec::add(phases, filePhase, phases);

        zeropadDFT.polarToReal(mags, phases, convBuffer);

        Vec::copy(convBuffer.sub(frameSize, frameSize), outputFrame);
    });

    // aout.print();
    return OK;
}
