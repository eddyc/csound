//
//  UKFFormant.hpp
//  KalmanFormantOpcode
//
//  Created by Edward Costello on 27/08/2017.
//  Copyright © 2017 Edward Costello. All rights reserved.
//

#include "../new_common/DATFile.hpp"
#include "../new_common/DFT.hpp"
#include "../new_common/FrameBuffer.hpp"
#include "../new_common/NetCDFFile.hpp"
#include "../new_common/Vector.hpp"
#include <Accelerate/Accelerate.h>
#include <functional>
#include <plugin.h>
using namespace std;
typedef Vector<MYFLT> Vec;
typedef Matrix<MYFLT> Mat;

#ifndef _FastConvolve_
#define _FastConvolve_
class FastConvolve : public csnd::Plugin<1, 5>
{
    std::function<MYFLT*(size_t)> allocator;
    std::string filename;
    Vec ain, aout, mags, phases, window, convBuffer, convTail;
    Mat timeDomainDAT, fileMags, filePhases;
    FrameBuffer<MYFLT> frameBuffer;
    DFT dft, zeropadDFT;
    size_t frameSize;
    size_t convSize;
    size_t hopSize;
    size_t channel;

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

    void allocateDataInputStructures(size_t frameSize, size_t rowCount);
    void openDat();
    void openSofa();
    void openSofaIR(NetCDFFile&);
    void openSofaTF(NetCDFFile&);
};

#endif
