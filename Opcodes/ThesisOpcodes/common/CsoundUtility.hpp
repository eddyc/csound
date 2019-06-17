//
//  CsoundUtility.hpp
//  CsoundDSPTools
//
//  Created by Edward Costello on 04/04/2017.
//  Copyright © 2017 Edward Costello. All rights reserved.
//

#include <stdio.h>
#include "SignalMaths.hpp"
#include <plugin.h>

class CsoundUtility {

public:

    static void allocateArray(csnd::Csound *csound, SignalData::Vector &dimensions, ARRAYDAT *arrayOutArg);
    static void pvsSplit(csnd::pv_frame &pvsData, SignalData::Vector &magnitudes, SignalData::Vector &frequencies);
    static void pvsJoin(SignalData::Vector &magnitudes, SignalData::Vector &frequencies, csnd::pv_frame &pvsData);
    static std::pair<csnd::AuxMem<MYFLT>, SignalData::Vector> allocateVector(csnd::Csound *csound, size_t elementCount);
    static void *allocate(csnd::Csound *csound, AUXCH *memory, size_t byteSize, size_t count);
};

