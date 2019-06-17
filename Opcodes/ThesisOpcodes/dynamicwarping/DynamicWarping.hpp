//
//  NewDynamicWarping.hpp
//  DTW
//
//  Created by Edward Costello on 01/06/2017.
//  Copyright © 2017 Edward Costello. All rights reserved.
//

#include "DifferenceMatrix.hpp"

#include <plugin.h>

#ifndef _DynamicWarping_
#define _DynamicWarping_
using namespace SignalData;

class DynamicWarping {

    const size_t modelFrameCount;
    const size_t paletteSegmentCount;
    size_t currentFrame;
    void incrementFrame();
    Vector pTime, qTime, timeWarpPath, globalTimeSegmentCosts,
        globalFrequencySegmentCosts;
    Vector pFreq, qFreq, freqWarpPath, freqDelta;

    PVSegment transposePaletteOutput;
    PVSegment paletteOutputCache;
    const ComparisonMode mode;

  public:
    DifferenceMatrix *differenceMatrices;
    AUXCH differenceMatricesMemory;
    DynamicWarping(csnd::Csound *csound, PVSegment &paletteData,
                   size_t modelFrameCount, size_t paletteSegmentCount,
                   ComparisonMode mode);
    bool writeFrame(Vector &inputFrame);
    void traceWarpPath(Matrix &globalCostPath, Vector &p, Vector &q,
                       Vector &warpPath);
    void timeWarpPaletteToOutput(DifferenceMatrix *bestTimeMatch,
                                 PVSegment &paletteOutput);
    void timeWarpPaletteToOutput(PVSegment &input, PVSegment &paletteOutput);
    void getBestMatchingSegment(DifferenceMatrix *&bestMatch, MYFLT &cost,
                                Vector &globalCosts,
                                MYFLT (*getGlobalCost)(DifferenceMatrix &));
    void frequencyWarpPaletteToOutput(DifferenceMatrix *bestMatch,
                                      PVSegment &paletteOutput);

    void warpPaletteToOutput(PVSegment &paletteOutput);
};

#endif
