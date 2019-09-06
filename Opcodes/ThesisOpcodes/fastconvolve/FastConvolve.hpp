//
//  UKFFormant.hpp
//  KalmanFormantOpcode
//
//  Created by Edward Costello on 27/08/2017.
//  Copyright © 2017 Edward Costello. All rights reserved.
//

#include "./common/Vector.hpp"
#include <Accelerate/Accelerate.h>
#include <functional>
#include <plugin.h>
#include <samplerate.h>

using namespace std;
typedef Vector<MYFLT> Vec;
class FastConvolve : public csnd::Plugin<1, 3> {
    VectorFactory<MYFLT> newVec;
    Vec ain;

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
