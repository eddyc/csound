//
//  UKFFormant.hpp
//  KalmanFormantOpcode
//
//  Created by Edward Costello on 27/08/2017.
//  Copyright © 2017 Edward Costello. All rights reserved.
//

#include "../common/FileIO.hpp"
#include "../common/FrameBuffer.hpp"
#include "../common/RingBuffer.hpp"
#include "../common/SignalData/DFT.hpp"
#include "../common/SignalData/Matrix.hpp"
#include "../common/SignalData/Plotter.hpp"
#include "../common/SignalData/Vector.hpp"
#include "../common/json.hpp"
#include <plugin.h>

using namespace Signals;

typedef Matrix<double> Mat;
typedef Matrix<float> Matf;
typedef Vector<double> Vec;
typedef Vector<float> Vecf;

class ModVoc : public csnd::Plugin<1, 1> {
  public:
    DFT<MYFLT> dft;
    Vec input, output, window;
    FrameBuffer<MYFLT> frameBuffer;
    CSOUND *cs;
    size_t windowSize, hopSize, ksmps;
    Plotter<double> plotter;
    int init();
    void setWindow();
    static int init_(CSOUND *csound, ModVoc *self) {
        self->init();
        return OK;
    }
    int kperf();
    int _kperf();
    static int kperf_(CSOUND *csound, ModVoc *self) {
        self->kperf();
        return OK;
    }
};
