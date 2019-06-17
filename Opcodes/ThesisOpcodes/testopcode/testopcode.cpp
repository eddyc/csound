//
//  UKFFormant.cpp
//  KalmanFormantOpcode
//
//  Created by Edward Costello on 27/08/2017.
//  Copyright © 2017 Edward Costello. All rights reserved.
//
#include "testopcode.hpp"
#include "../common/SignalData/Matrix.hpp"
#include <assert.h>
#include <iostream>
#include <string>
#include <fstream>      // std::ifstream
#include <vector>
#include "../common/json.hpp"


using namespace Signals;
int TestOpcode::init()
{
    printf("Initialised\n");
    
    double data1[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    double data2[6] = {7, 8, 9, 10, 11, 12};
    double data3[12] = {0};
    
    auto inputA = Matrix<double>(data1, 4, 2);
    auto inputB = Matrix<double>(data2, 3, 2);
    auto output = Matrix<double>(data3, 4, 3);

    Matrix<double>::multiplyTransposed(inputA, inputB, output);
    
    output.print();
    
    return OK;
    
}

int TestOpcode::kperf()
{
    //    printf("It's happening!!!\n");
    return OK;
}
