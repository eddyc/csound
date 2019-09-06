//
//  UKFFormant.hpp
//  KalmanFormantOpcode
//
//  Created by Edward Costello on 27/08/2017.
//  Copyright © 2017 Edward Costello. All rights reserved.
//

#include "../common/FrameBuffer.hpp"
#include "../common/SignalData/DFT.hpp"
#include "../common/SignalData/Matrix.hpp"
#include "../common/SignalData/Vector.hpp"
#include "NetCDFFile.hpp"
#include <Accelerate/Accelerate.h>
#include <plugin.h>
#include <samplerate.h>

using namespace Signals;

using namespace std;
typedef Vector<double> Vec;
typedef Matrix<double> Mat;

class SOFAFile {
public:
    int dimensionCount;
    int variableCount;
    int attributeCount;
    int unlimitedDimensionsID;
};

class SofaOpcode : public csnd::Plugin<1, 3> {
    Vec input, output, window, inMags, inPhases, frequencyScale;
    function<MYFLT*(size_t)> allocator;
    Mat fileMags, filePhases;
    FrameBuffer<MYFLT> frameBuffer;
    DFT dft;
    string filename;

    void sofaToPolar(NetCDFFile& input, Mat& mags, Mat& phases, Mat& temp);
    void openSofa();
    void openDat();

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
