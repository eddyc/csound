//
//  UKFFormant.hpp
//  KalmanFormantOpcode
//
//  Created by Edward Costello on 27/08/2017.
//  Copyright © 2017 Edward Costello. All rights reserved.
//

#include "../common/FrameBuffer.hpp"
#include "../common/SignalData/Matrix.hpp"
#include "../common/SignalData/Vector.hpp"
#include "NetCDFFile.hpp"
#include <Accelerate/Accelerate.h>
#include <plugin.h>
#include <samplerate.h>

using namespace Signals;

using namespace std;
typedef Vector<double> Vec;
class SofaOpcode : public csnd::Plugin<1, 1> {
    Vec input, output, window, inMags, inPhases, interlacedPolar;
    // CSOUND* cs;
    // ARRAYDAT* input;
    // ARRAYDAT* output;
    // float *inputBuffer, outputBuffer;
    // int inputArgumentCount;
    // int outputArgumentCount;
    // int error;
    // SRC_STATE* srcState;
    // SRC_DATA srcData;
    NetCDFFile fileL, fileR;
    FrameBuffer<MYFLT> frameBuffer;
    FFTSetupD fftSetup;
    DSPDoubleSplitComplex zInput;

    void realToPolar(Vec& inputFrame);
    void polarToReal(Vec& inputFrame);

public:
    int init();
    static int init_(CSOUND* csound, SofaOpcode* self)
    {
        self->init();
        return OK;
    }
    int kperf();
    static int kperf_(CSOUND* csound, SofaOpcode* self)
    {
        self->kperf();
        return OK;
    }
};
