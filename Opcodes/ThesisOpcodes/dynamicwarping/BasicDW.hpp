//
//  NewDW.hpp
//  BasicDW
//
//  Created by Edward Costello on 01/06/2017.
//  Copyright © 2017 Edward Costello. All rights reserved.
//

#include <plugin.h>
#include "DynamicWarping.hpp"

using namespace SignalData;


class BasicDW : public csnd::Plugin <2, 4>
{
    Matrix inputModelData;
    PVSegment inputPaletteData;
    PVSegment outputPaletteData;
    ComparisonMode mode;
    DynamicWarping dynamicWarping;

public:
    int init();
    void process();
};
