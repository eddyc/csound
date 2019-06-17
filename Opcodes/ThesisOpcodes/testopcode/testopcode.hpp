//
//  UKFFormant.hpp
//  KalmanFormantOpcode
//
//  Created by Edward Costello on 27/08/2017.
//  Copyright © 2017 Edward Costello. All rights reserved.
//

#import <plugin.h>

class TestOpcode : public csnd::Plugin <1, 1>  {

public:
    int init();
    static int init_(CSOUND *csound, TestOpcode *self) {
        self->init();
        return OK;
    }
    int kperf();
    static int kperf_(CSOUND *csound, TestOpcode *self) {
        self->kperf();
        return OK;
    }
};
