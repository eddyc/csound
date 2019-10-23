//
//  UKFFormant.hpp
//  KalmanFormantOpcode
//
//  Created by Edward Costello on 27/08/2017.
//  Copyright © 2017 Edward Costello. All rights reserved.
//

#include "../new_common/DFT.hpp"
#include "../new_common/FrameBuffer.hpp"
#include "../new_common/Plot.hpp"
#include "LogYPsd.hpp"
#include <plugin.h>
using namespace std;
typedef Vector<MYFLT> Vec;
typedef Matrix<MYFLT> Mat;

class ModVoc : public csnd::Plugin<1, 1> {
public:
    FrameBuffer<MYFLT> frameBuffer;
    Vec ain, aout, magnitudes, phases, window, audiofile, real, imag;
    function<MYFLT*(size_t)> allocator;
    LogYPsd<MYFLT> logYPsd;
    Plot<double> plot;
    DFT dft;
    size_t frameSize;
    size_t convSize;
    size_t hopSize;
    size_t channel;
    size_t currentSample;

    int init();
    void setWindow(Vec&);
    static int init_(CSOUND* csound, ModVoc* self)
    {
        self->init();
        return OK;
    }
    int kperf();
    static int kperf_(CSOUND* csound, ModVoc* self)
    {
        self->kperf();
        return OK;
    }
};
