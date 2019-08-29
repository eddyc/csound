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
const int log2n = 7;
const int n = frameSize;

using namespace std;

int SofaOpcode::init()
{
    CSOUND* cs = (CSOUND*)csound;

    function<MYFLT*(size_t)> allocator = [&](size_t count) -> MYFLT* {
        return (MYFLT*)(cs->Calloc(cs, sizeof(MYFLT) * count));
    };

    fftSetup = vDSP_create_fftsetupD(log2n, kFFTRadix2);
    zInput.realp = (double*)allocator(n / 2);
    zInput.imagp = (double*)allocator(n / 2);
    const int ksmps = cs->GetKsmps(cs);
    double scalerStep = 1. / (n / 2 + 1);

    new (&input) Vec(((MYFLT*)inargs.data(0)), ksmps);
    new (&output) Vec(((MYFLT*)outargs.data(0)), ksmps);
    new (&window) Vec(allocator(frameSize), frameSize);
    new (&inMags) Vec(allocator(n / 2 + 1), n / 2 + 1);
    new (&inPhases) Vec(allocator(n / 2 + 1), n / 2 + 1);
    new (&frequencyScale) Vec(allocator(n / 2 + 1), n / 2 + 1);
    new (&interlacedPolar) Vec(allocator(n + 2), n + 2);
    STRINGDAT* filenameDat = (STRINGDAT*)(inargs.data(1));
    string filename = filenameDat->data;
    frequencyScale.ramp(1, -scalerStep);

    window.hanningWindow();
    file = NetCDFFile(filename, "TF");

    Mat tempInterlacedPolar(allocator(file.M * file.N * 2), file.M, file.N * 2);
    new (&fileMags) Mat(allocator(file.M * file.N), file.M, file.N);
    new (&filePhases) Mat(allocator(file.M * file.N), file.M, file.N);
    sofaToPolar(file, fileMags, filePhases, tempInterlacedPolar);

    new (&frameBuffer) FrameBuffer<MYFLT>(ksmps, frameSize, hopSize, allocator);

    return OK;
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

int SofaOpcode::kperf()
{
    CSOUND* cs = (CSOUND*)csound;

    Vec in = Vec((MYFLT*)input.data, cs->GetKsmps(cs));
    Vec out = Vec((MYFLT*)output.data, cs->GetKsmps(cs));
    int index = *((MYFLT*)inargs.data(2));

    cout << index << endl;
    Vec mags = fileMags[index];
    Vec phi = filePhases[index];

    frameBuffer.process(in, out, [&](Vec inputFrame, Vec outputFrame) {
        Vec::multiply(inputFrame, window, inputFrame);
        realToPolar(inputFrame);

        Vec::multiply(inMags, mags, inMags);
        Vec::add(inPhases, phi, inPhases);

        polarToReal(inputFrame);
        Vec::copy(inputFrame, outputFrame);
        outputFrame.multiply(0.5);
    });

    return OK;
}

void SofaOpcode::realToPolar(Vec& inputFrame)
{
    vDSP_ctozD((DSPDoubleComplex*)inputFrame.data, 2, &zInput, 1, n / 2);
    vDSP_fft_zripD(fftSetup, &zInput, 1, log2n, kFFTDirection_Forward);
    zInput.realp[n / 2] = zInput.imagp[0];
    zInput.imagp[n / 2] = 0;
    zInput.imagp[0] = 0;

    vDSP_ztocD(&zInput, 1, (DSPDoubleComplex*)interlacedPolar.data, 2, n / 2 + 1);
    vDSP_polarD(interlacedPolar.data, 2, (double*)interlacedPolar.data, 2, n / 2 + 1);

    cblas_dcopy((UInt32)n / 2 + 1, &interlacedPolar.data[0], 2, (double*)inMags.data, 1);
    cblas_dcopy((UInt32)n / 2 + 1, &interlacedPolar.data[1], 2,
                (double*)inPhases.data, 1);
}

void SofaOpcode::polarToReal(Vec& inputFrame)
{
    cblas_dcopy((UInt32)n / 2 + 1, inMags.data, 1, (double*)&interlacedPolar.data[0], 2);
    cblas_dcopy((UInt32)n / 2 + 1, inPhases.data, 1, (double*)&interlacedPolar.data[1], 2);

    vDSP_rectD(interlacedPolar.data, 2, (double*)interlacedPolar.data, 2, n / 2 + 1);
    vDSP_ctozD((DSPDoubleComplex*)interlacedPolar.data, 2, &zInput, 1, n / 2 + 1);

    zInput.imagp[0] = zInput.realp[n / 2];
    zInput.realp[n / 2] = 0;
    zInput.imagp[n / 2] = 0;

    vDSP_fft_zripD(fftSetup, &zInput, 1, log2n, kFFTDirection_Inverse);
    vDSP_ztocD(&zInput, 1, (DSPDoubleComplex*)inputFrame.data, 2, n / 2);

    inputFrame.multiply(0.5 / n);
}
