
//
//  module.cpp
//  Csound
//
//  Created by Edward Costello on 18/10/2017.
//

#include "FastConvolve.hpp"
extern "C" {
OENTRY localops[] = {{(char*)"fastconvolve", sizeof(FastConvolve), 0, 3,
                      (char*)"a", (char*)"aSkSi", (SUBR)FastConvolve::init_,
                      (SUBR)FastConvolve::kperf_}};

PUBLIC int csoundModuleInit_fastconvolve(CSOUND* csound)
{
    int status = csound->AppendOpcode(
        csound, localops[0].opname, localops[0].dsblksiz, localops[0].flags,
        localops[0].thread, localops[0].outypes, localops[0].intypes,
        (SUBR)localops[0].iopadr, (SUBR)localops[0].kopadr, (SUBR)localops[0].aopadr);
    return status;
}
#ifndef INIT_STATIC_MODULES
PUBLIC int csoundModuleCreate(CSOUND* csound)
{
    csnd::plugin<FastConvolve>((csnd::Csound*)csound,
                               (const char*)"fastconvolve", (const char*)"a",
                               (const char*)"aSkSi", (uint32_t)csnd::thread::ik);
    return 0;
}

PUBLIC int csoundModuleInit(CSOUND* csound)
{
    return csoundModuleInit_fastconvolve(csound);
}

PUBLIC int csoundModuleDestroy(CSOUND* csound)
{
    return 0;
}
#endif

LINKAGE
}
