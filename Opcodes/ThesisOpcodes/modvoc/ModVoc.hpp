//
//  UKFFormant.hpp
//  KalmanFormantOpcode
//
//  Created by Edward Costello on 27/08/2017.
//  Copyright © 2017 Edward Costello. All rights reserved.
//

#include "../new_common/FrameBuffer.hpp"
#include <plugin.h>
using namespace std;
typedef Vector<MYFLT> Vec;
typedef Matrix<MYFLT> Mat;

class ModVoc : public csnd::Plugin<1, 1> {
public:
    FrameBuffer<MYFLT> frameBuffer;
    Vec ain, aout;
    std::function<MYFLT*(size_t)> allocator;

    // DFT dft, zeropadDFT;
    size_t frameSize;
    size_t convSize;
    size_t hopSize;
    size_t channel;



    int init();
    void setWindow();
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
