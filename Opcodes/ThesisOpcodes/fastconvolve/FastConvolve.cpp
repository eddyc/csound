//
//  UKFFormant.cpp
//  Signals
//
//  Created by Edward Costello on 24/01/2018.
//  Copyright © 2018 Edward Costello. All rights reserved.
//

#include "FastConvolve.hpp"
#include <fstream>
#include <netcdf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

int FastConvolve::init()
{
    newVec = VectorFactory<MYFLT>([&](size_t count) -> MYFLT* {
        CSOUND* cs = (CSOUND*)csound;
        return (MYFLT*)(cs->Calloc(cs, sizeof(MYFLT) * count));
    });
    const auto data = newVec(10);
    const auto filler = newVec(5).fill(4);

    data.push(filler);
    data.print();
    return OK;
}

int FastConvolve::kperf()
{
    CSOUND* cs = (CSOUND*)csound;

    const int ksmps = cs->GetKsmps(cs);

    new (&ain) Vec(((MYFLT*)inargs.data(0)), ksmps);

    ain.fill(44);
    // CSOUND* cs = (CSOUND*)csound;

    return OK;
}
