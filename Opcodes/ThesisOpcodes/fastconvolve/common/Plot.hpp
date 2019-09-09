//
//  Plot.h
//  ModulationVocoder
//
//  Created by Edward Costello on 28/07/2014.
//  Copyright (c) 2014 Edward Costello. All rights reserved.
//

#include "Vector.hpp"
#include <MacTypes.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#ifndef _Plot_
#define _Plot_
template <typename T>
class Plot {
public:
    static std::ofstream openFile(const char* fileName);
    static void closeFile(std::ofstream& tmp);
    static void command(const std::string& cmdstr, int size, ...);
    static void x(Vector<T> input);
    static void image(Matrix<T> input, int offsetX, int offsetY);
    static void image(Matrix<T> inputA, int offsetX, int offsetY, Vector<T> inputB);
};

#endif
