//
//  module.cpp
//  Csound
//
//  Created by Edward Costello on 18/10/2017.
//

#include "UKFFormant.hpp"

extern "C" {
    OENTRY localops[] = {{(char *)"ukfformant",
        sizeof(UKFFormant),
        0,
        3,
        (char *)"a[]a[]a",
        (char *)"ai[]i[]i",
        (SUBR)UKFFormant::init_,
        (SUBR)UKFFormant::kperf_}};
    
    PUBLIC int csoundModuleInit_ukfformant(CSOUND *csound) {
        
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
        
        csnd::plugin<UKFFormant>((csnd::Csound *)csound, (const char *)"ukfformant",
                                 (const char *)"a[]a[]a", (const char *)"ai[]i[]i",
                                 (uint32_t)csnd::thread::ik);
        return 0;
    }
    
    PUBLIC int csoundModuleInit(CSOUND *csound) {
        return csoundModuleInit_ukfformant(csound);
    }
    
    PUBLIC int csoundModuleDestroy(CSOUND *csound) { return 0; }
#endif
    
    LINKAGE
}

