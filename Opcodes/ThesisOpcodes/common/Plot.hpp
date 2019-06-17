//
//  Plot.h
//  ModulationVocoder
//
//  Created by Edward Costello on 28/07/2014.
//  Copyright (c) 2014 Edward Costello. All rights reserved.
//


#include <MacTypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdarg.h>
#include "SignalData.hpp"



class Plot {
  
public:
    
    static std::ofstream openFile(const char *fileName);
    static void closeFile(std::ofstream &tmp);
    static void command(const std::string &cmdstr, int size, ...);
    static void x(SignalData::Vector input);
    static void image(SignalData::Matrix input, int offsetX, int offsetY);
    static void image(SignalData::Matrix inputA, int offsetX, int offsetY, SignalData::Vector inputB);
};
