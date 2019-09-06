//
//  UKFFormant.cpp
//  Signals
//
//  Created by Edward Costello on 24/01/2018.
//  Copyright © 2018 Edward Costello. All rights reserved.
//

#include "SofaOpcodes.hpp"
#include "utilities.hpp"
#include <fstream>
#include <netcdf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int frameSize = 128;
const int hopSize = frameSize / 4;
const int log2n = 7;
const int n = frameSize;

using namespace std;

int SofaOpcode::init()
{
    CSOUND* cs = (CSOUND*)csound;

    allocator = [&](size_t count) -> MYFLT* {
        return (MYFLT*)(cs->Calloc(cs, sizeof(MYFLT) * count));
    };

    new (&dft) DFT(allocator, log2n);
    const int ksmps = cs->GetKsmps(cs);
    double scalerStep = 1. / (n / 2 + 1);

    new (&input) Vec(((MYFLT*)inargs.data(0)), ksmps);
    new (&output) Vec(((MYFLT*)outargs.data(0)), ksmps);
    new (&window) Vec(allocator(frameSize), frameSize);
    new (&inMags) Vec(allocator(n / 2 + 1), n / 2 + 1);
    new (&inPhases) Vec(allocator(n / 2 + 1), n / 2 + 1);
    new (&frequencyScale) Vec(allocator(n / 2 + 1), n / 2 + 1);
    STRINGDAT* filenameDat = (STRINGDAT*)(inargs.data(1));
    filename = filenameDat->data;
    frequencyScale.ramp(1, -scalerStep);

    window.hanningWindow();

    if ((filename.substr(filename.find_last_of(".") + 1) == "sofa")) {
        openSofa();
    }
    else if ((filename.substr(filename.find_last_of(".") + 1) == "dat")) {
        openDat();
    }
    else {
        cout << "Probably not the right file type" << endl;
        exit(-1);
    }
    new (&frameBuffer) FrameBuffer<MYFLT>(ksmps, frameSize, hopSize, allocator);

    return OK;
}

void SofaOpcode::openDat()
{
    streampos begin, end;
    ifstream file(filename, ios::binary);
    begin = file.tellg();
    file.seekg(0, ios::end);
    end = file.tellg();
    const size_t sizeBytes = end - begin;
    const size_t elementCount = sizeBytes / sizeof(float);
    const size_t rowCount = elementCount / 128;
    Mat allFileData = Mat(allocator(elementCount), rowCount, 128);
    file.seekg(0, ios::beg);
    float fileInput[elementCount];
    file.read((char*)fileInput, sizeBytes);
    double* allFileDataDouble = (double*)allFileData.data;

    for (size_t i = 0; i < elementCount; ++i) {
        allFileDataDouble[i] = fileInput[i];
    }
    file.close();

    Mat tempMags(allocator(rowCount * 64), rowCount, 64);
    new (&fileMags) Mat(allocator(rowCount * 65), rowCount, 65);
    cblas_dcopy(64 * rowCount, (double*)allFileData.data, 2, (double*)tempMags.data, 1);

    vDSP_mmovD(tempMags.data, (double*)fileMags.data, 64, rowCount, 64, 65);
    Mat tempPhases(allocator(rowCount * 64), rowCount, 64);
    new (&filePhases) Mat(allocator(rowCount * 65), rowCount, 65);
    cblas_dcopy(64 * rowCount, (double*)&allFileData.data[1], 2,
                (double*)tempPhases.data, 1);
    vDSP_mmovD(tempPhases.data, (double*)filePhases.data, 64, rowCount, 64, 65);
    cblas_dcopy(rowCount, tempPhases.data, 64, (double*)&fileMags.data[64], 65);
    vDSP_vclrD((double*)filePhases.data, 65, rowCount);
}

void SofaOpcode::sofaToPolar(NetCDFFile& input, Mat& mags, Mat& phases, Mat& temp)
{
    DSPDoubleSplitComplex tempComplex;
    tempComplex.realp = &input.dataRealValues[0];
    tempComplex.imagp = &input.dataImagValues[0];
    vDSP_ztocD(&tempComplex, 1, (DSPDoubleComplex*)temp.data, 2, temp.elementCount / 2);
    vDSP_polarD(temp.data, 2, (double*)temp.data, 2, temp.elementCount / 2);

    cblas_dcopy((UInt32)temp.elementCount / 2, &temp.data[0], 2, (double*)mags.data, 1);
    cblas_dcopy((UInt32)temp.elementCount / 2, &temp.data[1], 2, (double*)phases.data, 1);

    double max = mags.max();
    mags.divide(max);

    for (int i = 0; i < mags.rowCount; ++i) {
        Vec row = mags[i];
        Vec::multiply(row, frequencyScale, row);
    }
}

void SofaOpcode::openSofa()
{
    NetCDFFile file = NetCDFFile(filename, "TF");

    Mat tempInterlacedPolar(allocator(file.M * file.N * 2), file.M, file.N * 2);
    new (&fileMags) Mat(allocator(file.M * file.N), file.M, file.N);
    new (&filePhases) Mat(allocator(file.M * file.N), file.M, file.N);
    sofaToPolar(file, fileMags, filePhases, tempInterlacedPolar);
}

int SofaOpcode::kperf()
{
    CSOUND* cs = (CSOUND*)csound;

    Vec in = Vec((MYFLT*)input.data, cs->GetKsmps(cs));
    Vec out = Vec((MYFLT*)output.data, cs->GetKsmps(cs));
    int index = *((MYFLT*)inargs.data(2));

    Vec mags = fileMags[index];
    Vec phi = filePhases[index];

    mags[0] = abs(mags[0]);
    mags[1] = abs(mags[1]);

    frameBuffer.process(in, out, [&](Vec inputFrame, Vec outputFrame) {
        Vec::multiply(inputFrame, window, inputFrame);
        dft.realToPolar(inputFrame, inMags, inPhases);

        Vec::multiply(inMags, mags, inMags);
        Vec::add(inPhases, phi, inPhases);

        dft.polarToReal(inMags, inPhases, inputFrame);
        Vec::copy(inputFrame, outputFrame);
        outputFrame.multiply(0.5);
    });

    return OK;
}
