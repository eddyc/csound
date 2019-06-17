//
//  DifferenceMatrix.hpp
//  DTW
//
//  Created by Edward Costello on 02/06/2017.
//  Copyright © 2017 Edward Costello. All rights reserved.
//

#include "../common/SignalMaths.hpp"
#include <plugin.h>

#ifndef _DifferenceMatrix_
#define _DifferenceMatrix_
class DynamicWarping;
using namespace SignalData;

typedef enum ComparisonMode {

  TIME,
  FREQUENCY,
  TIME_AND_FREQUENCY
} ComparisonMode;

class DifferenceMatrix : SignalMaths {

  friend DynamicWarping;
  const PVSegment paletteData;
  const PVSegment transposePaletteData;
  Vector frameDifference, frequencyDifference;
  size_t currentFrame;
  const size_t modelFrameCount;
  const size_t mode;
  void incrementFrame();
  void getFrameDifference(Vector &inputFrame);
  void getBandDifference(Vector &inputFrame);
  void getTimeCost();
  void getFrequencyCost();

public:
  Matrix timeCost, frequencyCost;
  Matrix timeCostPath, frequencyCostPath;
  DifferenceMatrix(csnd::Csound *csound, PVSegment &paletteData,
                   size_t modelFrameCount, ComparisonMode mode);
  bool writeFrame(Vector &inputFrame);
  static MYFLT getGlobalTimeCostValue(DifferenceMatrix &);
  static MYFLT getGlobalFrequencyCostValue(DifferenceMatrix &);
};
#endif
