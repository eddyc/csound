//
//  module.cpp
//  Csound
//
//  Created by Edward Costello on 18/10/2017.
//

#include "JsonInputOpcode.hpp"
extern "C" {
    OENTRY localops[] = {{(char *)"jsoninput",
        sizeof(JsonInputOpcode),
        0,
        3,
        (char *)"*",
        (char *)"*",
        (SUBR)JsonInputOpcode::init_,
        (SUBR)JsonInputOpcode::kperf_}};
    
    PUBLIC int csoundModuleInit_json(CSOUND *csound) {
        
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
        
        csnd::plugin<JsonInputOpcode>((csnd::Csound *)csound,
                                 (const char *)"jsoninput",
                                 (const char *)"*",
                                 (const char *)"*",
                                 (uint32_t)csnd::thread::ik);
        return 0;
    }
    
    PUBLIC int csoundModuleInit(CSOUND *csound) {
        return csoundModuleInit_json(csound);
    }
    
    PUBLIC int csoundModuleDestroy(CSOUND *csound) { return 0; }
#endif
    
    LINKAGE
}

