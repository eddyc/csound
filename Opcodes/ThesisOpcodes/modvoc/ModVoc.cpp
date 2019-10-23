//
//  UKFFormant.cpp
//  Signals
//
//  Created by Edward Costello on 24/01/2018.
//  Copyright © 2018 Edward Costello. All rights reserved.
//

#include "ModVoc.hpp"
#include <sndfile.hh>
#include <stdlib.h>
int ModVoc::init()
{
    CSOUND* cs = (CSOUND*)csound;

    const int ksmps = cs->GetKsmps(cs);
    const int fs = cs->GetSr(cs);
    allocator = [&](size_t count) -> MYFLT* {
        return (MYFLT*)(cs->Calloc(cs, sizeof(MYFLT) * count));
    };
    new (&ain) Vec(inargs.data(0), ksmps);
    new (&aout) Vec(outargs.data(0), ksmps);

    frameSize = pow(2, 15);
    hopSize = frameSize / 8;

    new (&magnitudes) Vec(allocator, frameSize / 2 + 1);
    new (&phases) Vec(allocator, frameSize / 2 + 1);
    new (&real) Vec(allocator, frameSize / 2 + 1);
    new (&imag) Vec(allocator, frameSize / 2 + 1);
    new (&window) Vec(allocator, frameSize);
    setWindow(window);

    new (&dft) DFT(allocator, log2(frameSize));
    new (&frameBuffer) FrameBuffer<MYFLT>(ksmps, hopSize, frameSize, allocator, true);

    string filePath = "/Users/eddyc/Desktop/input.wav";

    SndfileHandle soundfile(filePath);
    new (&audiofile) Vec(allocator, soundfile.frames());
    soundfile.read((double*)audiofile.data, (sf_count_t)audiofile.elementCount);
    currentSample = 0;

    new (&logYPsd) LogYPsd(allocator, fs, frameSize / 2 + 1, hopSize);

    return OK;
}

void ModVoc::setWindow(Vec& input)
{
    Vec tmp = Vec(allocator, input.elementCount / 2);
    tmp.sineWindow();
    Vec::multiply(tmp, tmp, tmp);
    Vec::copy(tmp.sub(tmp.elementCount / 2, 0),
              input.sub(input.elementCount / 4, input.elementCount * (1. / 8.)));
    input.sub(input.elementCount / 4, input.elementCount * (3. / 8.)).fill(1);
    Vec::copy(tmp.sub(tmp.elementCount / 2, tmp.elementCount / 2),
              input.sub(input.elementCount / 4, input.elementCount * (5. / 8.)));
}

int ModVoc::kperf()
{
    frameBuffer.process(ain, aout, [&](Vec input, Vec output) -> void {
        Vec::copy(audiofile.sub(input.elementCount, currentSample), input);
        currentSample += hopSize;

        Vec::multiply(input, window, input);

        dft.realToComplex(input, real, imag);
        real.multiply(0.5), imag.multiply(0.5);
        logYPsd(real, imag, magnitudes);

        dft.polarToReal(magnitudes, phases, output);
    });
    return OK;
}
