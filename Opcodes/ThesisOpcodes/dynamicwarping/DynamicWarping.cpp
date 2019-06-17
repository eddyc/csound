//
//  DynamicWarping.cpp
//  DTW
//
//  Created by Edward Costello on 01/06/2017.
//  Copyright © 2017 Edward Costello. All rights reserved.
//

#include "DynamicWarping.hpp"
#include "../common/CsoundUtility.hpp"
#include "../common/Plot.hpp"
#include <assert.h>

DynamicWarping::DynamicWarping(csnd::Csound *csound, PVSegment &paletteData,
                               size_t modelFrameCount,
                               size_t paletteSegmentCount, ComparisonMode mode)
: modelFrameCount(modelFrameCount),
paletteSegmentCount(paletteSegmentCount),
currentFrame(0),
pTime(csound, modelFrameCount * 2), qTime(csound, modelFrameCount * 2),
timeWarpPath(csound, modelFrameCount),
globalTimeSegmentCosts(csound, paletteSegmentCount),
globalFrequencySegmentCosts(csound, paletteSegmentCount),
pFreq(csound, paletteData.columnCount * 2),
qFreq(csound, paletteData.columnCount * 2),
freqWarpPath(csound, paletteData.columnCount),
freqDelta(csound, paletteData.columnCount - 1),
transposePaletteOutput(csound, paletteData.columnCount, modelFrameCount),
paletteOutputCache(csound, modelFrameCount, paletteData.columnCount),
mode(mode),
differenceMatrices((DifferenceMatrix *)CsoundUtility::allocate(csound, &differenceMatricesMemory, sizeof(DifferenceMatrix),
                                                               paletteSegmentCount)){

    size_t segmentIndexIncrement = paletteData.rowCount / paletteSegmentCount;
    for (size_t i = 0; i < paletteSegmentCount; ++i) {
        
        PVSegment currentSegment = paletteData.subSegment(
                                                          segmentIndexIncrement, i * segmentIndexIncrement);
        
        new (&differenceMatrices[i])
        DifferenceMatrix(csound, currentSegment, modelFrameCount, mode);
    }
}

void DynamicWarping::incrementFrame() {
    currentFrame++;
    currentFrame %= modelFrameCount;
}

void DynamicWarping::traceWarpPath(Matrix &globalCostPath, Vector &p, Vector &q,
                                   Vector &warpPath) {
    size_t i = globalCostPath.rowCount - 1;
    size_t j = globalCostPath.columnCount - 1;
    size_t index = 0;
    p[index] = i;
    q[index] = j;
    index++;
    
    while (i > 0 && j > 0) {
        size_t tb = globalCostPath[i][j];
        
        switch (tb) {
            case 1: {
                
                i--;
                j--;
                break;
            }
            case 2: {
                
                i--;
                break;
            }
            case 3: {
                
                j--;
                break;
            }
        }
        
        p[index] = i;
        q[index] = j;
        index++;
    }
    
    Vector pTrace = Vector(p, index);
    Vector qTrace = Vector(q, index);
    SignalMaths::reverse(pTrace);
    SignalMaths::reverse(qTrace);
    
    for (size_t i = 0; i < globalCostPath.rowCount; ++i) {
        
        for (size_t j = 0; j < pTrace.elementCount; ++j) {
            
            if (pTrace[j] >= i) {
                
                warpPath[i] = (size_t)qTrace[j];
                break;
            }
        }
    }
}

void DynamicWarping::timeWarpPaletteToOutput(DifferenceMatrix *bestMatch,
                                             PVSegment &paletteOutput) {
    PVSegment::copyRows((PVSegment &)bestMatch->paletteData, timeWarpPath,
                        paletteOutput);
}

void DynamicWarping::timeWarpPaletteToOutput(PVSegment &input,
                                             PVSegment &paletteOutput) {
    PVSegment::copyRows(input, timeWarpPath, paletteOutput);
}

void DynamicWarping::frequencyWarpPaletteToOutput(DifferenceMatrix *bestMatch,
                                                  PVSegment &paletteOutput) {
    for (size_t i = 0; i < freqWarpPath.elementCount; ++i) {
        
        size_t warpIndex = freqWarpPath[i];
        PVFrame paletteBand = bestMatch->transposePaletteData[warpIndex];
        PVFrame transposedPaletteOutputBand = transposePaletteOutput[i];
        
        PVFrame::copyData(paletteBand, transposedPaletteOutputBand);
        //        MYFLT delta = i == 0 ? 1 : (MYFLT)warpIndex / (MYFLT)i;
        //        SignalMaths::multiply(transposedPaletteOutputBand.frequencies,
        //        delta);
    }
    
    SignalMaths::transpose(transposePaletteOutput, paletteOutput);
}

void DynamicWarping::getBestMatchingSegment(
                                            DifferenceMatrix *&bestMatch, MYFLT &cost, Vector &globalCosts,
                                            MYFLT (*getGlobalCost)(DifferenceMatrix &input)) {
    for (size_t i = 0; i < paletteSegmentCount; ++i) {
        
        globalCosts[i] = getGlobalCost(differenceMatrices[i]);
    }
    
    auto minimum = SignalMaths::minimum(globalCosts);
    
    bestMatch = &differenceMatrices[minimum.first];
    cost = minimum.second;
}

bool DynamicWarping::writeFrame(Vector &inputFrame) {
    for (size_t i = 0; i < paletteSegmentCount; ++i) {
        
        differenceMatrices[i].writeFrame(inputFrame);
    }
    
    incrementFrame();
    
    return (currentFrame == 0);
}

void DynamicWarping::warpPaletteToOutput(PVSegment &paletteOutput) {
    auto getTimeWarpPath = [this](MYFLT &cost) {
        
        DifferenceMatrix *bestTimeMatch = nullptr;
        getBestMatchingSegment(bestTimeMatch, cost, globalTimeSegmentCosts,
                               DifferenceMatrix::getGlobalTimeCostValue);
        traceWarpPath(bestTimeMatch->timeCostPath, pTime, qTime, timeWarpPath);
        return bestTimeMatch;
    };
    
    auto getFreqWarpPath = [this](MYFLT &cost) {
        
        DifferenceMatrix *bestFreqMatch = nullptr;
        getBestMatchingSegment(bestFreqMatch, cost, globalFrequencySegmentCosts,
                               DifferenceMatrix::getGlobalFrequencyCostValue);
        traceWarpPath(bestFreqMatch->frequencyCostPath, pFreq, qFreq, freqWarpPath);
        return bestFreqMatch;
    };
    
    MYFLT cost = 0;
    DifferenceMatrix *bestMatch = nullptr;
    
    if (ComparisonMode::FREQUENCY == mode) {
        bestMatch = getFreqWarpPath(cost);
        frequencyWarpPaletteToOutput(bestMatch, paletteOutput);
    } else if (ComparisonMode::TIME == mode) {
        
        bestMatch = getTimeWarpPath(cost);
        timeWarpPaletteToOutput(bestMatch, paletteOutput);
    } else if (ComparisonMode::TIME_AND_FREQUENCY == mode) {
        
        MYFLT timeCost = 0, freqCost = 0;
        DifferenceMatrix *bestFreqMatch = getFreqWarpPath(freqCost);
        DifferenceMatrix *bestTimeMatch = getTimeWarpPath(timeCost);
        bestMatch = (timeCost < freqCost) ? bestTimeMatch : bestFreqMatch;
        
        frequencyWarpPaletteToOutput(bestMatch, paletteOutputCache);
        timeWarpPaletteToOutput(paletteOutputCache, paletteOutput);
    }
}
