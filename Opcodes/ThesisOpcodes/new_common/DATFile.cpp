#include "DATFile.hpp"
#include <fstream>
#include <netcdf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

DATFile::DATFile(function<double*(size_t)> allocator, string path)
    : rowCount(368),
      columnCount(128),
      magnitudes(allocator, rowCount, (columnCount / 2 + 1)),
      phases(allocator, rowCount, (columnCount / 2 + 1))
{
    streampos begin, end;
    ifstream file(path, ios::binary);
    begin = file.tellg();
    file.seekg(0, ios::end);
    end = file.tellg();
    const size_t sizeBytes = end - begin;
    const size_t elementCount = sizeBytes / sizeof(float);
    Mat allFileData = Mat(allocator, rowCount, columnCount);
    file.seekg(0, ios::beg);
    float fileInput[elementCount];
    file.read((char*)fileInput, sizeBytes);
    double* allFileDataDouble = (double*)allFileData.data;

    for (size_t i = 0; i < elementCount; ++i) {
        allFileDataDouble[i] = fileInput[i];
    }
    file.close();

    Mat tempMags(allocator, rowCount, (columnCount / 2));
    Mat tempPhases(allocator, rowCount, (columnCount / 2));
    new (&magnitudes) Mat(allocator, rowCount, (columnCount / 2 + 1));
    new (&phases) Mat(allocator, rowCount, (columnCount / 2 + 1));

    cblas_dcopy((columnCount / 2) * rowCount, (double*)allFileData.data, 2,
                (double*)tempMags.data, 1);

    vDSP_mmovD(tempMags.data, (double*)magnitudes.data, (columnCount / 2),
               rowCount, (columnCount / 2), (columnCount / 2 + 1));
    cblas_dcopy((columnCount / 2) * rowCount, (double*)&allFileData.data[1], 2,
                (double*)tempPhases.data, 1);
    vDSP_mmovD(tempPhases.data, (double*)phases.data, (columnCount / 2),
               rowCount, (columnCount / 2), (columnCount / 2 + 1));
    cblas_dcopy(rowCount, tempPhases.data, (columnCount / 2),
                (double*)&magnitudes.data[(columnCount / 2)], (columnCount / 2 + 1));
    vDSP_vclrD((double*)phases.data, (columnCount / 2 + 1), rowCount);
}
