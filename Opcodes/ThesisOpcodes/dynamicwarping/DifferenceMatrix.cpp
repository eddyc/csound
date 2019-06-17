//
//  DifferenceMatrix.cpp
//  DTW
//
//  Created by Edward Costello on 02/06/2017.
//  Copyright © 2017 Edward Costello. All rights reserved.
//

#include "DifferenceMatrix.hpp"
#include <assert.h>

void initialiseCost(Matrix &cost);

DifferenceMatrix::DifferenceMatrix(csnd::Csound *csound, PVSegment &paletteData,
                                   size_t modelFrameCount, ComparisonMode mode)
:
paletteData(PVSegment::interpolate(csound, paletteData, modelFrameCount)),
transposePaletteData(csound, paletteData.columnCount, modelFrameCount),
frameDifference(csound, paletteData.columnCount),
frequencyDifference(csound, paletteData.columnCount), currentFrame(0),
modelFrameCount(modelFrameCount),
mode(mode),
timeCost(csound, modelFrameCount + 1, modelFrameCount + 1),
frequencyCost(csound, paletteData.columnCount + 1,
              paletteData.columnCount + 1),
timeCostPath(csound, modelFrameCount, modelFrameCount),
frequencyCostPath(csound, paletteData.columnCount, paletteData.columnCount)
{
    SignalMaths::transpose(this->paletteData, transposePaletteData);
    
    initialiseCost(timeCost);
}

void initialiseCost(Matrix &cost) {
    
    SignalMaths::fill(cost, 0);
    SignalMaths::fillColumn(cost, 0, INF);
    SignalMaths::fillRow(cost, 0, INF);
    cost[(size_t)0][(size_t)0] = 0;
}

void DifferenceMatrix::incrementFrame() {
    currentFrame++;
    currentFrame %= paletteData.rowCount;
}

void DifferenceMatrix::getFrameDifference(Vector &inputFrame) {
    Vector timeCostRow =
    timeCost[currentFrame + 1].subVector(1, timeCost.columnCount - 1);
    
    for (size_t i = 0; i < paletteData.rowCount; ++i) {
        
        Vector paletteRow = ((Matrix)paletteData.magnitudes)[i];
        substract(paletteRow, inputFrame, frameDifference);
        square(frameDifference);
        timeCostRow[i] = sum(frameDifference);
    }
}

void DifferenceMatrix::getBandDifference(Vector &inputFrame) {
    Vector paletteFrame = ((Matrix)(paletteData.magnitudes))[currentFrame];
    
    for (size_t i = 0; i < paletteData.columnCount; ++i) {
        
        Vector frequencyCostRow =
        frequencyCost[i + 1].subVector(1, frequencyCost.columnCount - 1);
        MYFLT binValue = inputFrame[i];
        
        substract(paletteFrame, binValue, frequencyDifference);
        square(frequencyDifference);
        SignalMaths::add(frequencyDifference, frequencyCostRow, frequencyCostRow);
    }
}

void DifferenceMatrix::getTimeCost() {
    for (size_t i = 0; i < timeCostPath.columnCount; ++i) {
        
        MYFLT minArrayData[3];
        Vector minArray = Vector(*minArrayData, 3);
        size_t minIndex;
        MYFLT minValue;
        
        minArray[(size_t)0] = timeCost[currentFrame][i];
        minArray[(size_t)1] = timeCost[currentFrame][i + 1];
        minArray[(size_t)2] = timeCost[currentFrame + 1][i];
        
        std::tie(minIndex, minValue) = minimum(minArray);
        timeCost[currentFrame + 1][i + 1] += minValue;
        timeCostPath[currentFrame][i] = minIndex + 1;
    }
}

void DifferenceMatrix::getFrequencyCost() {
    for (size_t row = 0; row < frequencyCostPath.rowCount; ++row) {
        
        for (size_t column = 0; column < frequencyCostPath.columnCount; ++column) {
            
            MYFLT minArrayData[3];
            Vector minArray = Vector(*minArrayData, 3);
            size_t minIndex;
            MYFLT minValue;
            
            minArray[(size_t)0] = frequencyCost[row][column];
            minArray[(size_t)1] = frequencyCost[row][column + 1];
            minArray[(size_t)2] = frequencyCost[row + 1][column];
            
            std::tie(minIndex, minValue) = minimum(minArray);
            frequencyCost[row + 1][column + 1] += minValue;
            frequencyCostPath[row][column] = minIndex + 1;
        }
    }
}

bool DifferenceMatrix::writeFrame(Vector &inputFrame) {
    assert(inputFrame.elementCount == paletteData.columnCount);
    
    if (currentFrame == 0) {
        
        if (ComparisonMode::FREQUENCY == mode ||
            ComparisonMode::TIME_AND_FREQUENCY == mode) {
            
            SignalMaths::fill(frequencyCost, 0);
            initialiseCost(frequencyCost);
        }
    }
    
    if (ComparisonMode::TIME == mode ||
        ComparisonMode::TIME_AND_FREQUENCY == mode) {
        
        getFrameDifference(inputFrame);
        getTimeCost();
    }
    
    if (ComparisonMode::FREQUENCY == mode ||
        ComparisonMode::TIME_AND_FREQUENCY == mode) {
        
        getBandDifference(inputFrame);
    }
    
    incrementFrame();
    
    if (currentFrame == 0) {
        
        if (ComparisonMode::FREQUENCY == mode ||
            ComparisonMode::TIME_AND_FREQUENCY == mode) {
            
            getFrequencyCost();
        }
        
        return true;
    } else {
        
        return false;
    }
}

MYFLT DifferenceMatrix::getGlobalTimeCostValue(DifferenceMatrix &input) {
    return input.timeCost[input.modelFrameCount][input.modelFrameCount];
}

MYFLT DifferenceMatrix::getGlobalFrequencyCostValue(DifferenceMatrix &input) {
    return input.frequencyCost[input.frequencyCost.rowCount - 1]
    [input.frequencyCost.rowCount - 1];
}
