//
//  NewMatrix.hpp
//  DTW
//
//  Created by Edward Costello on 01/03/2017.
//  Copyright © 2017 Edward Costello. All rights reserved.
//

#include <plugin.h>
#include <map>
#include <string>


#define _USE_HDF5_
//#define _USE_ACCELERATE_

#ifdef _USE_HDF5_
extern "C" {
#include "hdf5.h"
}
#endif
#ifndef _SignalData_
#define _SignalData_

class VectorRingbuffer;
class DynamicWarping;
class DTW;
class CompositeRow;
class CsoundUtility;
class Plot;

namespace SignalData {

class Matrix;
class Cube;
class SignalMaths;
class MatrixArray;
class CsVector;
class PVSegment;

class Vector {

    friend SignalMaths;
    friend Matrix;
    friend Cube;
    friend VectorRingbuffer;
    friend DynamicWarping;
    friend DTW;
    friend CompositeRow;
    friend MatrixArray;
    friend CsoundUtility;
    friend Plot;
    friend PVSegment;
    AUXCH memory;
    MYFLT const *data;
    const int stride;
    
    

  public:
    const size_t elementCount;
    const size_t allocatedElementCount;
    Vector();
    Vector(csnd::Csound *csound, size_t elementCount);
    Vector(ARRAYDAT *input);
    Vector(MYFLT &data, size_t elementCount);
    Vector(MYFLT &data, size_t elementCount, int stride);
    Vector(Vector &vector, size_t elementCount);

    MYFLT &operator[](size_t index);
    MYFLT operator[](MYFLT index);
    void operator*=(MYFLT value);
    Vector &operator=(Vector other);
    Vector subVector(size_t offset, size_t length) const;
    void print() const;
    void print(bool) const;
    void plot();
    void clear();
    static void copy(Vector &input, Vector &output);
    static void copy(Vector &input, size_t elementCount, size_t inputOffset,
                     Vector &output, size_t outputOffset);
    Matrix toMatrix(size_t rowCount, size_t columnCount);
    
#ifdef _USE_HDF5_
    void save(std::string path, std::string dataset) const;
    Vector(csnd::Csound *csound, std::string path, std::string dataSet);
    void saveData(std::string path, std::string dataset,
                  hsize_t dimensions[2]) const;
#endif
};

class PVFrame {

    friend Vector;

  public:
    const Vector magnitudes;
    const Vector frequencies;
    PVFrame(const Vector &magnitudes, const Vector &frequencies);
    PVFrame(ARRAYDAT *magnitudes, ARRAYDAT *frequencies);
    PVFrame(csnd::Csound *csound, size_t elementCount);
    PVFrame subBand(size_t offset, size_t length) const;
    void plot() const;
    static void copyData(PVFrame &input, PVFrame &output);
};

class Matrix : public Vector {

    friend SignalMaths;
    friend DynamicWarping;
    friend DTW;
    friend Cube;
    friend CompositeRow;
    friend PVSegment;

  public:
    Matrix();
    Matrix(ARRAYDAT *);
    Matrix(csnd::Csound *csound, size_t dimensionCount);
    Matrix(csnd::Csound *csound, size_t rowCount, size_t columnCount);
    Matrix(MYFLT &data, size_t rowCount, size_t columnCount);
    Matrix(Vector &data, size_t rowCount, size_t columnCount);
    Vector operator[](size_t index) const;
    Matrix &operator=(Matrix other);
    void getRow(MYFLT index, Vector &row);
    const Vector diagonal() const;
    void print() const;
    void print(bool) const;
    void plot() const;
    void plot(int offsetX, int offsetY);
    void save(std::string path, std::string dataset) const;
    static void copy(const Matrix &input, size_t inputRowCount,
                     size_t inputRowOffset, size_t inputColumnCount,
                     size_t inputColumnOffset, const Matrix &output,
                     size_t outputRowOffset, size_t outputColumnOffset);
    static void copyColumns(const Matrix &input, size_t columnCount,
                            size_t columnOffset, const Matrix &output);
    static void copyColumns(const Matrix &input, size_t inputColumnCount,
                            size_t inputColumnOffset, const Matrix &output,
                            size_t outputColumnOffset);
    static void copyRows(Matrix &input, Vector &specifiedRows, Matrix &output);
    static void copyRows(Matrix &input, size_t rowCount, size_t rowOffset,
                         Matrix &output);
    void repeatRows(Vector &input);
    const size_t rowCount;
    const size_t columnCount;
    void resize(size_t rowCount, size_t columnCount);
    static bool equalDimensions(Matrix &inputA, Matrix &inputB);
    Matrix subMatrix(size_t rowOffset, size_t rowCount);

    static SignalData::Matrix
    dataSetToMatrix(const char *path, const char *dataSet, MYFLT **memory);
    static MYFLT dataSetToScalar(const char *path, const char *dataSet);
};

class PVSegment {

    friend SignalMaths;

  public:
    const Matrix magnitudes;
    const Matrix frequencies;
    const size_t rowCount;
    const size_t columnCount;

    PVSegment();
    PVSegment(const Matrix &magnitudes, const Matrix &frequencies);
    PVSegment(ARRAYDAT *magnitudes, ARRAYDAT *frequencies);
    PVSegment(size_t rowCount, size_t columnCount, MYFLT *magnitudeMemory,
              MYFLT *frequencyMemory);
    PVSegment(csnd::Csound *csound, size_t rowCount, size_t columnCount);
    PVSegment operator=(PVSegment);
    PVFrame operator[](size_t index) const;
    static void copyRows(PVSegment &input, Vector &specifiedRows,
                         PVSegment &output);
    PVSegment subSegment(size_t rowCount, size_t rowOffset);
    static void copyColumns(PVSegment &input, size_t inputColumnCount,
                            size_t inputColumnOffset, PVSegment &output);
    static void copyColumns(PVSegment &input, size_t inputColumnCount,
                            size_t inputColumnOffset, PVSegment &output,
                            size_t outputColumnOffset);

    static PVSegment interpolate(csnd::Csound *csound, PVSegment input,
                                 size_t rowCount);
    void plot();
};

class MatrixArray : public Vector {
    const Vector rowCounts;
    const Vector columnCounts;
    const Vector offsets;
    const size_t matrixCount;
    size_t getMatrixArrayElementCount(Vector &rowCounts, Vector &columnCounts);

  public:
    MatrixArray(MYFLT &matrixDataMem, Vector &rowCounts, Vector &columnCounts,
                Vector &offsets);
    Matrix operator[](size_t index);
    MatrixArray &operator=(MatrixArray other);
};

class Cube : public Vector {

    friend SignalMaths;
    friend DynamicWarping;
    friend DTW;
    friend CompositeRow;

    const size_t sliceCount;
    const size_t rowCount;
    const size_t columnCount;

  public:
    Cube();
    Cube(MYFLT &data, size_t sliceCount, size_t rowCount, size_t columnCount);
    Matrix operator[](size_t index);
    Cube &operator=(Cube other);
    void print();
};
}

#endif
