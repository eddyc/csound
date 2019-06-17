//
//  module.cpp
//  Csound
//
//  Created by Edward Costello on 18/10/2017.
//

#include "testopcode.hpp"

extern "C" {
    OENTRY localops[] = {{(char *)"testopcode",
        sizeof(TestOpcode),
        0,
        3,
        (char *)"k",
        (char *)"k",
        (SUBR)TestOpcode::init_,
        (SUBR)TestOpcode::kperf_}};

    PUBLIC int csoundModuleInit_testopcode(CSOUND *csound) {


        int status = csound->AppendOpcode(csound,
                                          localops[0].opname,
                                          localops[0].dsblksiz,
                                          localops[0].flags,
                                          localops[0].thread,
                                          localops[0].outypes,
                                          localops[0].intypes,
                                          (SUBR)localops[0].iopadr,
                                          (SUBR)localops[0].kopadr,
                                          (SUBR)localops[0].aopadr);
        return status;
    }
#ifndef INIT_STATIC_MODULES
    PUBLIC int csoundModuleCreate(CSOUND *csound) {

        csnd::plugin<TestOpcode>((csnd::Csound *)csound, (const char *)"testopcode",
                                 (const char *)"k", (const char *)"k",
                                 (uint32_t)csnd::thread::ik);
        return 0;
    }

    PUBLIC int csoundModuleInit(CSOUND *csound) {
        return csoundModuleInit_testopcode(csound);
    }

    PUBLIC int csoundModuleDestroy(CSOUND *csound) { return 0; }
#endif

    LINKAGE
}
