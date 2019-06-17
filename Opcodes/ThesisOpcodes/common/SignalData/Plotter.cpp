//
//  Plot.c
//  ModulationVocoder
//
//  Created by Edward Costello on 28/07/2014.
//  Copyright (c) 2014 Edward Costello. All rights reserved.
//

#include "Plotter.hpp"

static FILE *gnucmd = popen("gnuplot", "w");
const char *fileName = "/Volumes/RAMDisk/currentPlot";
const char *fileNameA = "/Volumes/RAMDisk/currentPlotA";
const char *fileNameB = "/Volumes/RAMDisk/currentPlotB";

template <typename T>
void Plotter<T>::command(const std::string &cmdstr, int size, ...) {
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

template <typename T>
std::ofstream Plotter<T>::openFile(const char *fileNameIn) {
    std::ofstream tmp;

    tmp.open(fileNameIn);

    if (tmp.bad()) {

        std::cerr << "Cannot create temorary file: exiting plot" << std::endl;
        exit(-1);
    }

    return tmp;
}

template <typename T> void Plotter<T>::closeFile(std::ofstream &tmp) {
    tmp.flush();
    tmp.close();
}

template <typename T> void Plotter<T>::x(Vector<T> input) {
    T min = input[0];
    T max = input[0];

    for (int i = 0; i < input.elementCount; i++) {
        T value = input[(T)i];

        if (value > max) {
            max = value;
        }

        if (value < min) {
            min = value;
        }
    }

    x(input, min, max);
}

template <typename T> void Plotter<T>::x(Vector<T> input, T min, T max) {
    std::ostringstream cmdstr;
    std::ofstream tmp = openFile(fileName);

    for (int i = 0; i < input.elementCount; i++) {
        T value = input[(T)i];

        tmp << value << std::endl;
    }

    closeFile(tmp);

    cmdstr << "set yrange [" << min << ":" << max << "];" << std::endl;
    std::cout << cmdstr.str() << std::endl;
    cmdstr << "plot ";
    cmdstr << "\"" << fileName << "\" with lines";

    command(cmdstr.str(), 0);
}

template <typename T>
void Plotter<T>::image(Matrix<T> input, int offsetX, int offsetY) {
    std::ostringstream cmdstr;
    std::ofstream tmp = openFile(fileName);

    for (int i = offsetY; i < input.rowCount; ++i) {

        for (int j = offsetX; j < input.columnCount; ++j) {

            tmp << input[i][(T)j] << " ";
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

template <typename T>
void Plotter<T>::image(Matrix<T> inputA, int offsetX, int offsetY,
                       Vector<T> inputB) {
    std::ostringstream cmdstr;
    std::ofstream tmpA = openFile(fileNameA);
    std::ofstream tmpB = openFile(fileNameB);

    for (int i = offsetY; i < inputA.rowCount; ++i) {

        for (int j = offsetX; j < inputA.columnCount; ++j) {

            tmpA << inputA[i][(T)j] << " ";
        }

        tmpA << std::endl;
    }

    closeFile(tmpA);
    for (int i = 0; i < inputB.elementCount; ++i) {

        tmpB << inputB[(T)i] << std::endl;
    }

    closeFile(tmpB);

    cmdstr << "set palette rgbformulae 33,13,10" << std::endl;
    cmdstr << "set style line 1 lc rgb '#ffffff' lt 1 lw 1.5" << std::endl;
    cmdstr << "set autoscale fix" << std::endl;
    cmdstr << "plot \"" << fileNameA << "\" matrix with image, \"" << fileNameB
           << "\" w lines ls 1";

    command(cmdstr.str(), 0);
}

template class Plotter<double>;
template class Plotter<float>;
