//
//  Plot.c
//  ModulationVocoder
//
//  Created by Edward Costello on 28/07/2014.
//  Copyright (c) 2014 Edward Costello. All rights reserved.
//

#include "Plot.hpp"

static FILE *gnucmd = popen("gnuplot", "w");
const char *fileName = "/Volumes/RAMDisk/currentPlot";
const char *fileNameA = "/Volumes/RAMDisk/currentPlotA";
const char *fileNameB = "/Volumes/RAMDisk/currentPlotB";

void Plot::command(const std::string &cmdstr, int size, ...) {
    va_list ap;
    char local_cmd[1024];

    va_start(ap, size);
    vsprintf(local_cmd, cmdstr.c_str(), ap);
    va_end(ap);
    strcat(local_cmd, "\n");
    fputs(local_cmd, gnucmd);
    fflush(gnucmd);
    return;
}

std::ofstream Plot::openFile(const char *fileNameIn) {
    std::ofstream tmp;

    tmp.open(fileNameIn);

    if (tmp.bad()) {

        std::cerr << "Cannot create temorary file: exiting plot" << std::endl;
        exit(-1);
    }

    return tmp;
}

void Plot::closeFile(std::ofstream &tmp) {
    tmp.flush();
    tmp.close();
}

void Plot::x(SignalData::Vector input) {
    std::ostringstream cmdstr;
    std::ofstream tmp = openFile(fileName);

    for (int i = 0; i < input.elementCount; i++) {

        tmp << input[(MYFLT)i] << std::endl;
    }

    closeFile(tmp);

    cmdstr << "plot ";
    cmdstr << "\"" << fileName << "\" with lines";

    command(cmdstr.str(), 0);
}

void Plot::image(SignalData::Matrix input, int offsetX, int offsetY) {
    std::ostringstream cmdstr;
    std::ofstream tmp = openFile(fileName);

    for (int i = offsetY; i < input.rowCount; ++i) {

        for (int j = offsetX; j < input.columnCount; ++j) {

            tmp << input[i][(MYFLT)j] << " ";
        }

        tmp << std::endl;
    }

    closeFile(tmp);

    cmdstr << "set palette rgbformulae 33,13,10" << std::endl;
    cmdstr << "set  grid front" << std::endl;
    cmdstr << "set autoscale fix" << std::endl;
    cmdstr << "plot ";
    cmdstr << "\"" << fileName << "\" matrix with image";

    command(cmdstr.str(), 0);
}

void Plot::image(SignalData::Matrix inputA, int offsetX, int offsetY,
                 SignalData::Vector inputB) {
    std::ostringstream cmdstr;
    std::ofstream tmpA = openFile(fileNameA);
    std::ofstream tmpB = openFile(fileNameB);

    for (int i = offsetY; i < inputA.rowCount; ++i) {

        for (int j = offsetX; j < inputA.columnCount; ++j) {

            tmpA << inputA[i][(MYFLT)j] << " ";
        }

        tmpA << std::endl;
    }

    closeFile(tmpA);
    for (int i = 0; i < inputB.elementCount; ++i) {

        tmpB << inputB[(MYFLT)i] << std::endl;
    }

    closeFile(tmpB);

    cmdstr << "set palette rgbformulae 33,13,10" << std::endl;
    cmdstr << "set style line 1 lc rgb '#ffffff' lt 1 lw 1.5" << std::endl;
    cmdstr << "set autoscale fix" << std::endl;
    cmdstr << "plot \"" << fileNameA << "\" matrix with image, \"" << fileNameB
           << "\" w lines ls 1";

    command(cmdstr.str(), 0);
}
