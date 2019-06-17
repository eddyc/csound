//
//  FFT.h
//  ModVoc
//
//  Created by Edward Costello on 24/08/2014.
//  Copyright (c) 2014 Edward Costello. All rights reserved.
//

#include "Matrix.hpp"
#include <Accelerate/Accelerate.h>
#include <MacTypes.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef _DSPFFT
#define _DSPFFT

using namespace Signals;
using namespace std;
template <typename T> class DFT {

    size_t FFTFrameSize;
    int log2n;
    FFTSetupD fftSetupD;
    FFTSetup fftSetup;
    DSPDoubleSplitComplex complexInputBufferD;
    DSPDoubleSplitComplex complexOutputBufferD;
    DSPSplitComplex complexInputBuffer;
    DSPSplitComplex complexOutputBuffer;

  public:
    DFT<T>(size_t FFTFrameSize, function<T *(size_t)> allocator);
    void forward(Vector<T> &input, ComplexVector<T> &output);
    void inverse(ComplexVector<T> &input, Vector<T> &output);
};

#endif
