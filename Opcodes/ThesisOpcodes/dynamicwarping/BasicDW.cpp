//
//  NewDW.cpp
//  BasicDW
//
//  Created by Edward Costello on 01/06/2017.
//  Copyright © 2017 Edward Costello. All rights reserved.
//

#include "BasicDW.hpp"
#include "../common/CsoundUtility.hpp"
#include <assert.h>

int BasicDW::init() {

    new (&inputModelData) Matrix((ARRAYDAT *)inargs.data(0));
    new (&inputPaletteData)
        PVSegment((ARRAYDAT *)inargs.data(1), (ARRAYDAT *)inargs.data(2));

    mode = (ComparisonMode)*inargs.data(3);

    MYFLT dimensionsData[2] = {(MYFLT)inputModelData.rowCount,
                               (MYFLT)inputModelData.columnCount};

    Vector dimensions = Vector(*dimensionsData, 2);
    CsoundUtility::allocateArray(csound, dimensions,
                                 (ARRAYDAT *)outargs.data(0));
    CsoundUtility::allocateArray(csound, dimensions,
                                 (ARRAYDAT *)outargs.data(1));

    new (&outputPaletteData)
        PVSegment((ARRAYDAT *)outargs.data(0), (ARRAYDAT *)outargs.data(1));
    new (&dynamicWarping) DynamicWarping(csound, inputPaletteData,
                                         inputModelData.rowCount, 1, mode);

    process();

    return OK;
}

void BasicDW::process() {

    for (size_t i = 0; i < inputModelData.rowCount; ++i) {

        Vector inputRow = inputModelData[i];

        bool ready = dynamicWarping.writeFrame(inputRow);

        if (ready == true) {

            dynamicWarping.warpPaletteToOutput(outputPaletteData);
        }
    }
}
