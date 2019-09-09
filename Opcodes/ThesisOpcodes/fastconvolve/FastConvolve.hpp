//
//  UKFFormant.hpp
//  KalmanFormantOpcode
//
//  Created by Edward Costello on 27/08/2017.
//  Copyright © 2017 Edward Costello. All rights reserved.
//

#include "./common/FrameBuffer.hpp"
#include "./common/Vector.hpp"
#include <Accelerate/Accelerate.h>
#include <functional>
#include <plugin.h>
#include <samplerate.h>
using namespace std;
typedef Vector<MYFLT> Vec;

#ifndef _FastConvolve_
#define _FastConvolve_
class FastConvolve : public csnd::Plugin<1, 3> {
    VectorFactory<MYFLT> newVec;
    Vec ain, aout, buffer;
    FrameBuffer<MYFLT> frameBuffer;

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
