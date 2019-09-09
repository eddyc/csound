//
//  UKFFormant.hpp
//  KalmanFormantOpcode
//
//  Created by Edward Costello on 27/08/2017.
//  Copyright © 2017 Edward Costello. All rights reserved.
//

#include "./common/DATFile.hpp"
#include "./common/DFT.hpp"
#include "./common/FrameBuffer.hpp"
#include "./common/Vector.hpp"
#include <Accelerate/Accelerate.h>
#include <functional>
#include <plugin.h>
#include <samplerate.h>
using namespace std;
typedef Vector<MYFLT> Vec;
typedef Matrix<MYFLT> Mat;

#ifndef _FastConvolve_
#define _FastConvolve_
class FastConvolve : public csnd::Plugin<1, 3> {
    VectorFactory<MYFLT> newVec;
    function<MYFLT*(size_t)> allocator;
    string filename;
    Vec ain, aout, mags, phases, window, convBuffer, convTail;
    Mat timeDomainDAT, fileMags, filePhases;
    FrameBuffer<MYFLT> frameBuffer;
    DFT dft, zeropadDFT;
    DATFile datfile;
    size_t frameSize;
    size_t convSize;
    size_t hopSize;

public:
    int init();
    static int init_(CSOUND* csound, FastConvolve* self)
    {
        self->init();
        return OK;
    }
    int kperf();
    static int kperf_(CSOUND* csound, FastConvolve* self)
    {
        self->kperf();
        return OK;
    }
};

#endif
