//
//  FramedDW.cpp
//  DynamicWarpingOpcode
//
//  Created by Edward Costello on 03/07/2017.
//  Copyright © 2017 Edward Costello. All rights reserved.
//

#include "FramedDW.hpp"
#include "../common/CsoundUtility.hpp"
#include <assert.h>

int FramedDW::init() {
    new (&inputModelData) Vector((ARRAYDAT *)inargs.data(0));
    inputFrameCount = (size_t)*inargs.data(1);
    inputSegmentCount = (size_t)*inargs.data(2);
    bandCount = (size_t)*inargs.data(3);

    new (&inputPaletteData)
        PVSegment((ARRAYDAT *)inargs.data(4), (ARRAYDAT *)inargs.data(5));

    if (inputFrameCount == -1) {

        inputFrameCount = inputPaletteData.rowCount / inputSegmentCount;
    }

    assert(bandCount <= inputPaletteData.columnCount);

    currentFrame = 0;
    mode = (ComparisonMode)*inargs.data(6);

    MYFLT dimensionsData[1] = {(MYFLT)inputPaletteData.columnCount};

    Vector dimensions = Vector(*dimensionsData, 1);

    CsoundUtility::allocateArray(csound, dimensions,
                                 (ARRAYDAT *)outargs.data(0));
    CsoundUtility::allocateArray(csound, dimensions,
                                 (ARRAYDAT *)outargs.data(1));

    new (&outputPaletteData)
        PVFrame((ARRAYDAT *)outargs.data(0), (ARRAYDAT *)outargs.data(1));
    new (&cachePaletteData)
        PVSegment(csound, inputFrameCount, inputPaletteData.columnCount);

    dynamicWarping = (DynamicWarping *)CsoundUtility::allocate(
        csound, &dynamicWarpingMemory, sizeof(DynamicWarping), bandCount);
    inputPaletteBands = (PVSegment *)CsoundUtility::allocate(
        csound, &inputPaletteBandsMemory, sizeof(PVSegment), bandCount);
    cachePaletteBands = (PVSegment *)CsoundUtility::allocate(
        csound, &cachePaletteBandsMemory, sizeof(PVSegment), bandCount);

    new (&bandStartIndices) Vector(csound, bandCount);
    new (&bandWidths) Vector(csound, bandCount);

    for (size_t i = 0; i < bandCount; ++i) {

        bandStartIndices[i] =
            round((MYFLT)i / (MYFLT)bandCount * inputPaletteData.columnCount);
        size_t endIndex = round((MYFLT)(i + 1) / (MYFLT)bandCount *
                                inputPaletteData.columnCount);
        bandWidths[i] = endIndex - bandStartIndices[i];

        new (&inputPaletteBands[i])
            PVSegment(csound, inputPaletteData.rowCount, bandWidths[i]);

        PVSegment::copyColumns(inputPaletteData, bandWidths[i],
                               bandStartIndices[i], inputPaletteBands[i]);

        new (&cachePaletteBands[i])
            PVSegment(csound, inputFrameCount, bandWidths[i]);
        new (&dynamicWarping[i])
            DynamicWarping(csound, inputPaletteBands[i], inputFrameCount,
                           inputSegmentCount, mode);
    }

    return OK;
}

void FramedDW::incrementFrame() {
    currentFrame++;
    currentFrame %= inputFrameCount;
}

int FramedDW::kperf() {
    bool ready = false;

    for (size_t i = 0; i < bandCount; ++i) {

        Vector inputModelBand =
            inputModelData.subVector(bandStartIndices[i], bandWidths[i]);
        ready = dynamicWarping[i].writeFrame(inputModelBand);
    }

    PVFrame currentCacheFrame = cachePaletteData[currentFrame];
    PVFrame::copyData(currentCacheFrame, outputPaletteData);

    if (ready == true) {

        for (size_t i = 0; i < bandCount; ++i) {

            dynamicWarping[i].warpPaletteToOutput(cachePaletteBands[i]);
            PVSegment::copyColumns(cachePaletteBands[i], bandWidths[i], 0,
                                   cachePaletteData, bandStartIndices[i]);
        }
    }

    incrementFrame();
    return OK;
}
