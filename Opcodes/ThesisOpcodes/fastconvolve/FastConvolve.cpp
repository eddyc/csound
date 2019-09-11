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
    new (&ain) Vec(((MYFLT*)inargs.data(0)), ksmps);
    new (&aout) Vec(((MYFLT*)outargs.data(0)), ksmps);
    STRINGDAT* filenameDat = (STRINGDAT*)(inargs.data(1));
    filename = filenameDat->data;

    const string fileType = filename.substr(filename.find_last_of(".") + 1);

    if (fileType == "sofa") {
        openSofa();
    }
    else if (fileType == "dat") {
        openDat();
    }

    new (&frameBuffer) FrameBuffer<MYFLT>(ksmps, hopSize, frameSize, allocator, false);
    new (&mags) Vec(allocator, convSize / 2 + 1);
    new (&phases) Vec(allocator, convSize / 2 + 1);
    new (&convBuffer) Vec(allocator, convSize);
    new (&convTail) Vec(allocator, frameSize);

    return OK;
}

void FastConvolve::allocateDataInputStructures(size_t frameSize, size_t hrtfsInDatCount)
{
    this->frameSize = frameSize;
    convSize = frameSize * 2;
    hopSize = frameSize;
    new (&dft) DFT(allocator, (size_t)log2((float)frameSize));
    new (&zeropadDFT) DFT(allocator, (size_t)log2((float)convSize));
    new (&timeDomainDAT) Mat(allocator, hrtfsInDatCount, convSize);
    new (&fileMags) Mat(allocator, hrtfsInDatCount, convSize / 2 + 1);
    new (&filePhases) Mat(allocator, hrtfsInDatCount, convSize / 2 + 1);
    new (&window) Vec(allocator, frameSize);
    window.hanningWindow();
}

void FastConvolve::openDat()
{
    DATFile datfile(allocator, filename);
    allocateDataInputStructures(128, 368);

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
}

void FastConvolve::openSofa()
{
    STRINGDAT* typeDat = (STRINGDAT*)(inargs.data(3));
    string type = typeDat->data;
    channel = *(MYFLT*)inargs.data(4);

    NetCDFFile sofaFile(filename, type);

    if (type == "IR") {
        openSofaIR(sofaFile);
    }
    else if (type == "TF") {
        openSofaTF(sofaFile);
    }
}

void FastConvolve::openSofaIR(NetCDFFile& sofaFile)
{
    allocateDataInputStructures(sofaFile.N, sofaFile.M);

    Mat irData = Matrix<MYFLT>(allocator, sofaFile.M, sofaFile.N);
    for (int i = 0; i < irData.rowCount; ++i) {
        int index = i * (sofaFile.N * sofaFile.R) + (sofaFile.N * channel);
        const MYFLT* data = &sofaFile.dataIRValues[index];
        Vec sofaRow((MYFLT*)data, sofaFile.N);
        auto row = timeDomainDAT[i].sub(frameSize, 0);
        Vec::copy(sofaRow, row);
        Vec::multiply(row, window, row);
        auto fullRow = timeDomainDAT[i];
        auto fileMagRow = fileMags[i];
        auto filePhaseRow = filePhases[i];
        zeropadDFT.realToPolar(fullRow, fileMagRow, filePhaseRow);
    }
}

void getMirror(const Vec& original, const Vec& destination, MYFLT multiplier)
{
    Vec firstSofaHalf = original.sub(original.elementCount, 0);
    Vec secondSofaHalf = original.sub(original.elementCount - 2, 1);
    Vec firstTempHalf = destination.sub(original.elementCount, 0);
    Vec secondTempHalf = destination.sub(original.elementCount - 2, original.elementCount);
    Vec::copy(firstSofaHalf, firstTempHalf);
    Vec::copy(secondSofaHalf, secondTempHalf);
    secondTempHalf.reverse().multiply(multiplier);
}

void FastConvolve::openSofaTF(NetCDFFile& sofaFile)
{
    allocateDataInputStructures(sofaFile.N * 2 - 2, sofaFile.M);

    Mat irData = Mat(allocator, sofaFile.M, sofaFile.N * 2 - 2);
    Mat tempReal = Mat(allocator, sofaFile.M, sofaFile.N * 2 - 2);
    Mat tempImag = Mat(allocator, sofaFile.M, sofaFile.N * 2 - 2);
    Mat sofaReal = Mat(&sofaFile.dataRealValues[0], sofaFile.M, sofaFile.N);
    Mat sofaImag = Mat(&sofaFile.dataImagValues[0], sofaFile.M, sofaFile.N);

    for (int i = 0; i < irData.rowCount; ++i) {
        getMirror(sofaReal[i], tempReal[i], 1);
        getMirror(sofaImag[i], tempImag[i], -1);
        dft.inPlaceForwardComplex(tempReal[i], tempImag[i]);
        tempImag[i].print();
        Plot<MYFLT>::x(tempImag[i]);
    }
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
