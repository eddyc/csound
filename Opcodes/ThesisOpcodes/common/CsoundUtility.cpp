//
//  CsoundUtility.cpp
//  CsoundDSPTools
//
//  Created by Edward Costello on 04/04/2017.
//  Copyright © 2017 Edward Costello. All rights reserved.
//

#include "CsoundUtility.hpp"
#include <assert.h>

void CsoundUtility::allocateArray(csnd::Csound *csound, SignalData::Vector &dimensions, ARRAYDAT *arrayOutArg)
{
    arrayOutArg->dimensions = (int)dimensions.elementCount;
    arrayOutArg->sizes = (int *)csound->calloc(sizeof(int) * dimensions.elementCount);
    
    arrayOutArg->sizes[0] = (int)dimensions[(size_t)0];
    size_t elementCount = dimensions[(size_t)0];
    
    if (dimensions.elementCount > 1) {
        size_t i;
        for (i = 1; i < dimensions.elementCount; ++i) {
            
            arrayOutArg->sizes[i] = (int)dimensions[i];
            elementCount *= arrayOutArg->sizes[i];
        }
    }
    
    
    CS_VARIABLE *arrayVariable = arrayOutArg->arrayType->createVariable(csound, NULL);
    arrayOutArg->arrayMemberSize = arrayVariable->memBlockSize;
    arrayOutArg->data = (MYFLT *)csound->calloc(sizeof(MYFLT) * elementCount);
}

void CsoundUtility::pvsSplit(csnd::pv_frame &pvsData, SignalData::Vector &magnitudes, SignalData::Vector &frequencies)
{
    assert(magnitudes.elementCount == frequencies.elementCount);
    assert(magnitudes.elementCount == pvsData.nbins());
    
    for (size_t i = 0; i < pvsData.nbins(); i++) {
        
        magnitudes[i] = pvsData[(int)i].amp();
        frequencies[i] = pvsData[(int)i].freq();
    }
}

void CsoundUtility::pvsJoin(SignalData::Vector &magnitudes, SignalData::Vector &frequencies, csnd::pv_frame &pvsData)
{
    assert(magnitudes.elementCount == frequencies.elementCount);
    assert(magnitudes.elementCount == pvsData.nbins());
   
    for (size_t i = 0; i < pvsData.nbins(); i++) {
        
        pvsData[(int)i].amp(magnitudes[i]);
        pvsData[(int)i].freq(frequencies[i]);
    }
    
}

void *CsoundUtility::allocate(csnd::Csound *csound, AUXCH *memory, size_t byteSize, size_t count) {
    
    size_t bytes = count * byteSize;
    CSOUND *cs = (CSOUND *)csound;
    if (memory->auxp == nullptr || memory->size < bytes) {
        
        cs->AuxAlloc(cs, bytes, memory);
    }
    
    return memory->auxp;
}
