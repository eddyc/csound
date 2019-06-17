//
//  FramedDW.hpp
//  DynamicWarpingOpcode
//
//  Created by Edward Costello on 03/07/2017.
//  Copyright © 2017 Edward Costello. All rights reserved.
//

#include "DynamicWarping.hpp"
#include <plugin.h>

using namespace SignalData;

class FramedDW : public csnd::Plugin<2, 7> {
    size_t inputFrameCount;
    size_t inputSegmentCount;
    size_t bandCount;
    size_t currentFrame;
    Vector inputModelData;
    Vector bandStartIndices, bandWidths;
    PVSegment *inputPaletteBands;
    PVSegment inputPaletteData;
    PVSegment cachePaletteData;
    PVSegment *cachePaletteBands;
    PVFrame outputPaletteData;
    AUXCH cachePaletteBandsMemory;
    AUXCH inputPaletteBandsMemory;


    ComparisonMode mode;


    enum warpMode { warpTime, warpFreq, warpBoth };

    AUXCH dynamicWarpingMemory;
    DynamicWarping *dynamicWarping;
    void incrementFrame();

public:
    int init();
    static int init_(CSOUND *csound, FramedDW *self) {
        self->init();
        return OK;
    }
    int kperf();
    static int kperf_(CSOUND *csound, FramedDW *self) {
        self->kperf();
        return OK;
    }
};

