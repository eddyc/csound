//
//  module.cpp
//
//  Created by Edward Costello on 28/03/2017.
//  Copyright © 2017 Edward Costello. All rights reserved.
//

#include "FramedDW.hpp"

extern "C" {
    OENTRY localops[] = {{(char *)"frameddw",
        sizeof(FramedDW),
        0,
        3,
        (char *)"k[]k[]",
        (char *)"k[]iiik[]k[]i",
        (SUBR)FramedDW::init_,
        (SUBR)FramedDW::kperf_}};
    
    PUBLIC int csoundModuleInit_frameddw(CSOUND *csound) {
        
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
        
        csnd::plugin<FramedDW>((csnd::Csound *)csound, (const char *)"frameddw",
                               (const char *)"k[]k[]", (const char *)"k[]iiik[]k[]i",
                               (uint32_t)csnd::thread::ik);
        return 0;
    }
    
    PUBLIC int csoundModuleInit(CSOUND *csound) {
        return csoundModuleInit_frameddw(csound);
    }
    
    PUBLIC int csoundModuleDestroy(CSOUND *csound) { return 0; }
#endif
    
    LINKAGE
}

