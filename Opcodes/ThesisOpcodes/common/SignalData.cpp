//
//  NewMatrix.cpp
//  DTW
//
//  Created by Edward Costello on 01/03/2017.
//  Copyright © 2017 Edward Costello. All rights reserved.
//

#include <assert.h>
#include <stdexcept>
#include <Accelerate/Accelerate.h>
#include "CsoundUtility.hpp"
#include "SignalMaths.hpp"
#include "Plot.hpp"

using namespace SignalData;

#pragma mark - Vector -

Vector::Vector() :
data(nullptr),
stride(0),
elementCount(0),
allocatedElementCount(0)
{}

Vector::Vector(csnd::Csound *csound, size_t elementCount) :
data((MYFLT *)CsoundUtility::allocate(csound, &memory, sizeof(MYFLT), elementCount)),
stride(1),
elementCount(elementCount),
allocatedElementCount(elementCount)
{}

Vector::Vector(MYFLT &data, size_t elementCount) :
data(&data),
stride(1),
elementCount(elementCount),
allocatedElementCount(elementCount)
{}

Vector::Vector(MYFLT &data, size_t elementCount, int stride) :
data(&data),
stride(stride),
elementCount(elementCount),
allocatedElementCount(elementCount)
{}


Vector::Vector(Vector &vector, size_t elementCount) :
data(vector.data),
stride(1),
elementCount(elementCount),
allocatedElementCount(vector.allocatedElementCount)
{
}

void memcpys(MYFLT *dest, int dstride, const MYFLT *src, int sstride, size_t count)
{
    if (dstride == 1 && sstride == 1) {

        memcpy(dest, src, sizeof(MYFLT) * count);
    }
    else {
        
        for (size_t i = 0; i < count; ++i) {

            dest[dstride * i ] = src[i * sstride];
        }
    }
}

void mmov(const double *input,
          double *output,
          size_t ioColumnCount,
          size_t ioRowCount,
          size_t inputColumnCount,
          size_t outputColumnCount) {
#ifdef _USE_ACCELERATE_
    vDSP_mmovD(input, output, ioColumnCount, ioRowCount, inputColumnCount, outputColumnCount);
#else
    for (size_t r = 0; r < ioRowCount; ++r) {

        for (size_t c = 0; c < ioColumnCount; ++c) {
            
            output[r * outputColumnCount + c] = input[r * inputColumnCount + c];
        }
    }
#endif
}

Vector::Vector(ARRAYDAT *input) : Vector()
{
    assert(input->dimensions == 1);
    memcpy((void *)&data, &input->data, sizeof(MYFLT *));
    
    size_t elements = input->sizes[0];
    memcpy((void *)&elementCount, &elements, sizeof(size_t));
    
    int one = 1;
    memcpy((void *)&stride, &one, sizeof(int));
}

#ifdef _USE_HDF5_

hsize_t getElementCount(std::string path, std::string dataSet)
{
    herr_t status;
    hid_t fileID, dataSetID, dataSpaceID;
    hsize_t dimensions[2] = {0};
    
    fileID = H5Fopen(path.c_str(),
                     H5F_ACC_RDONLY,
                     H5P_DEFAULT);
    
    dataSetID = H5Dopen2(fileID, dataSet.c_str(), H5P_DEFAULT);
    dataSpaceID =  H5Dget_space(dataSetID);
    
    H5Sget_simple_extent_dims(dataSpaceID, dimensions, NULL);
    
    status = H5Sclose(dataSpaceID);
    status = H5Dclose(dataSetID);
    status = H5Fclose(fileID);
    
    return dimensions[0] * dimensions[1];
}

Vector::Vector(csnd::Csound *csound, std::string path, std::string dataSet) :
Vector(csound, getElementCount(path, dataSet))
{
    
    hid_t fileID, dataSetID, dataSpaceID;
    hsize_t dimensions[2] = {0};
    
    herr_t status;
    
    fileID = H5Fopen(path.c_str(),
                     H5F_ACC_RDONLY,
                     H5P_DEFAULT);
    
    dataSetID = H5Dopen2(fileID, dataSet.c_str(), H5P_DEFAULT);
    dataSpaceID =  H5Dget_space(dataSetID);
    
    H5Sget_simple_extent_dims(dataSpaceID, dimensions, NULL);
    
    status = H5Dread(dataSetID, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, (void *)data);
    
    status = H5Sclose(dataSpaceID);
    status = H5Dclose(dataSetID);
    status = H5Fclose(fileID);
}

void Vector::saveData(std::string path, std::string dataset, hsize_t datasetDimensions[2]) const
{
    if(path.compare("desktop") == 0
       ||
       path.compare("Desktop") == 0) {
        
        std::string newPath = "/Users/eddyc/Desktop" + dataset + ".h5";
        path = newPath;
    }
    
    hid_t fileID, dataSetID, dataSpaceID;
    herr_t status;
    
    fileID = H5Fcreate(path.c_str(),
                       H5F_ACC_TRUNC,
                       H5P_DEFAULT,
                       H5P_DEFAULT);
    
    
    int rank = 2;
    
    dataSpaceID = H5Screate_simple(rank, datasetDimensions, NULL);
    dataSetID = H5Dcreate(fileID, dataset.c_str(), H5T_NATIVE_DOUBLE, dataSpaceID, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    
    H5Dwrite (dataSetID, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
    status = H5Dclose(dataSetID);
    
    status = H5Sclose(dataSpaceID);
    status = H5Fclose(fileID);
}

void Vector::save(std::string path, std::string dataset) const
{
    hsize_t datasetDimensions[2];
    datasetDimensions[0] = 1;
    datasetDimensions[1] = elementCount;
    
    saveData(path, dataset, datasetDimensions);
}


#endif

MYFLT &Vector::operator[](size_t index)
{
    assert(index < elementCount);
    return (MYFLT &)data[index * stride];
}

MYFLT Vector::operator[](MYFLT index)
{
    assert(index <= elementCount - 1 && index >= 0);
    
    if (ceil(index) == index) {
        
        return data[(int)index * stride];
    }
    else {
        
        MYFLT modulo = fmod(index * stride, 1);
        MYFLT floorValue = data[(int)floor(index * stride)];
        MYFLT ceilValue = data[(int)ceil(index * stride)];
        MYFLT difference = ceilValue - floorValue;
        MYFLT result = floorValue + (difference * modulo);
        return result;
    }
}

Vector &Vector::operator=(Vector other)
{
    memcpy((void *)&data, &other.data, sizeof(MYFLT *));
    memcpy((void *)&elementCount, (void *)&other.elementCount, sizeof(size_t));
    memcpy((void *)&stride, &other.stride, sizeof(int));
    return *this;
}

void Vector::operator*=(MYFLT value)
{
    
}


Vector Vector::subVector(size_t offset, size_t length) const
{
    assert(length + offset <= elementCount);
    return Vector(*(MYFLT *)&data[offset], length);
}

void Vector::print() const
{
    printf("Vector::print:\n");
    
    for (size_t i = 0; i < elementCount; ++i) {
        
        printf("[%f]", ((Vector)*this)[i]);
    }
    
    printf("\n\n");
}

void Vector::print(bool condition) const
{
    if (condition == true) {
        print();
    }
}

void Vector::plot()
{
    Plot::x(*this);
}

void Vector::copy(Vector &input, Vector &output)
{
    assert(input.elementCount == output.elementCount);
    memcpys((MYFLT *)output.data, output.stride, input.data, input.stride, input.elementCount);
}

void Vector::copy(Vector &input, size_t elementCount, size_t inputOffset, Vector &output, size_t outputOffset)
{
    assert(input.elementCount >= elementCount + inputOffset);
    assert(output.elementCount >= elementCount + outputOffset);
    
    memcpys((MYFLT *)&output.data[outputOffset], output.stride, &input.data[inputOffset], input.stride, input.elementCount);
}

Matrix Vector::toMatrix(size_t rowCount, size_t columnCount)
{
    assert((rowCount == 1 && columnCount == elementCount)
           ||
           (columnCount == 1 && rowCount == elementCount));
    
    
    Matrix mat = Matrix();
    memcpy((void *)&mat.rowCount, &rowCount, sizeof(size_t));
    memcpy((void *)&mat.columnCount, &columnCount, sizeof(size_t));
    memcpy((void *)&mat.data, &data, sizeof(MYFLT *));
    
    return mat;
}

void Vector::clear()
{
    SignalMaths::clear(*this);
}

#pragma mark - PVFrame -

PVFrame::PVFrame(const Vector &magnitudes, const Vector &frequencies) :
magnitudes(magnitudes),
frequencies(frequencies)
{}

PVFrame::PVFrame(ARRAYDAT *magnitudes, ARRAYDAT *frequencies) :
PVFrame(Vector(magnitudes), Vector(frequencies))
{}

void PVFrame::copyData(PVFrame &input, PVFrame &output)
{
    assert(input.magnitudes.elementCount == output.magnitudes.elementCount);
    assert(input.frequencies.elementCount == output.frequencies.elementCount);
    
    Vector::copy((Vector &)input.magnitudes, (Vector &)output.magnitudes);
    Vector::copy((Vector &)input.frequencies, (Vector &)output.frequencies);
}

PVFrame PVFrame::subBand(size_t offset, size_t length) const
{
    return PVFrame(magnitudes.subVector(offset, length), frequencies.subVector(offset, length));
}

void PVFrame::plot() const
{
    Plot::x(magnitudes);
}



#pragma mark - Matrix -

Matrix::Matrix() :
Vector(),
rowCount(0),
columnCount(0)
{}


Matrix::Matrix(MYFLT &data, size_t rowCount, size_t columnCount) :
Vector(data, rowCount * columnCount),
rowCount(rowCount),
columnCount(columnCount)
{}

Matrix::Matrix(Vector &data, size_t rowCount, size_t columnCount) :
Vector(data, rowCount * columnCount),
rowCount(rowCount),
columnCount(columnCount)
{}


Matrix::Matrix(ARRAYDAT *input) : Matrix()
{
    assert(input->dimensions <= 2);
    memcpy((void *)&data, &input->data, sizeof(MYFLT *));
    
    size_t rows, columns;
    
    if (input->dimensions == 1) {
        
        rows = 1;
        columns = input->sizes[0];
    }
    else {
        
        rows = input->sizes[0];
        columns = input->sizes[1];
    }
    
    memcpy((void *)&rowCount, &rows, sizeof(size_t));
    memcpy((void *)&columnCount, &columns, sizeof(size_t));
    
    size_t elements = rows * columns;
    memcpy((void *)&elementCount, &elements, sizeof(size_t));
}

Matrix::Matrix(csnd::Csound *csound, size_t dimensionCount) :
SignalData::Vector(csound, dimensionCount * dimensionCount),
rowCount(dimensionCount),
columnCount(dimensionCount)
{}

Matrix::Matrix(csnd::Csound *csound, size_t rowCount, size_t columnCount) :
SignalData::Vector(csound, rowCount * columnCount),
rowCount(rowCount),
columnCount(columnCount)
{}

Vector Matrix::operator[](size_t index) const
{
    assert(index < rowCount);
    return Vector((MYFLT &)data[index * columnCount], columnCount);
}

void Matrix::getRow(MYFLT index, Vector &row)
{
    assert(index <= elementCount - 1 && index >= 0);
    assert(row.elementCount == columnCount);
    
    if (ceil(index) == index) {
        
        row = (*this)[(size_t)index];
    }
    else {
        
        MYFLT modulo = fmod(index, 1);
        Vector floorValue = (*this)[(int)floor(index)];
        Vector ceilValue = (*this)[(int)ceil(index)];
        SignalMaths::substract(floorValue, ceilValue, row);
        SignalMaths::multiply(row, modulo);
        SignalMaths::add(floorValue, row, row);
    }
}

const Vector Matrix::diagonal() const
{
    assert(columnCount == rowCount);
    return Vector((MYFLT &)*this->data, columnCount, columnCount + 1);
}
Matrix &Matrix::operator=(Matrix other)
{
    memcpy((void *)&data, &other.data, sizeof(MYFLT *));
    memcpy((void *)&elementCount, (void *)&other.elementCount, sizeof(size_t));
    memcpy((void *)&rowCount, (void *)&other.rowCount, sizeof(size_t));
    memcpy((void *)&columnCount, (void *)&other.columnCount, sizeof(size_t));
    return *this;
}

void Matrix::print() const
{
    printf("Matrix::print:\n");
    
    for (size_t i = 0; i < rowCount; ++i) {
        
        for (size_t j = 0; j < columnCount; ++j) {
            
            printf("[%g]", (*this)[i][j]);
        }
        
        printf("\n");
    }
    
    printf("\n");
}

void Matrix::print(bool condition) const
{
    if (condition == true) {
        print();
    }
}

void Matrix::plot() const
{
    Plot::image(*this, 0, 0);
}

void Matrix::plot(int offsetX, int offsetY)
{
    Plot::image(*this, offsetX, offsetY);
}

void Matrix::copy(const Matrix &input, size_t inputRowCount, size_t inputRowOffset,
                  size_t inputColumnCount, size_t inputColumnOffset,
                  const Matrix &output, size_t outputRowOffset, size_t outputColumnOffset)
{
    assert(inputRowCount + outputRowOffset <= output.rowCount);
    assert(inputColumnCount + outputColumnOffset <= output.columnCount);
    assert(inputRowCount + inputRowOffset <= input.rowCount);
    assert(inputColumnCount + inputColumnOffset <= input.columnCount);
    
    mmov((const MYFLT *)&input.data[inputColumnOffset + input.columnCount * inputRowOffset],
               (MYFLT *)&output.data[outputColumnOffset + output.columnCount * outputRowOffset],
               inputColumnCount,
               inputRowCount,
               input.columnCount,
               output.columnCount);
}

void Matrix::copyColumns(const Matrix &input, size_t columnCount, size_t columnOffset, const Matrix &output)
{
    assert(output.columnCount == columnCount);
    assert(input.columnCount >= columnCount + columnOffset);
    
    mmov((const MYFLT *)&input.data[columnOffset],
               (MYFLT *)output.data,
               columnCount,
               input.rowCount,
               input.columnCount,
               output.columnCount);
}

void Matrix::copyColumns(const Matrix &input, size_t inputColumnCount, size_t inputColumnOffset,
                         const Matrix &output, size_t outputColumnOffset)
{
    assert(input.columnCount >= inputColumnCount + inputColumnOffset);
    assert(output.columnCount >= inputColumnCount + outputColumnOffset);
    
    mmov((const MYFLT *)&input.data[inputColumnOffset],
               (MYFLT *)&output.data[outputColumnOffset],
               inputColumnCount,
               input.rowCount,
               input.columnCount,
               output.columnCount);
}

void Matrix::copyRows(Matrix &input, size_t rowCount, size_t rowOffset, Matrix &output)
{
    assert(input.columnCount == output.columnCount);
    assert(rowCount + rowOffset <= input.rowCount);
    assert(rowCount + rowOffset == output.rowCount);
    
    
}

void Matrix::copyRows(Matrix &input, Vector &specifiedRows, Matrix &output)
{
    assert(specifiedRows.elementCount == output.rowCount);
    assert(input.columnCount == output.columnCount);
    
    for (size_t i = 0; i < specifiedRows.elementCount; ++i) {
        
        size_t rowNumber = specifiedRows[i];
        assert(rowNumber < input.rowCount);
        Vector inputVector = input[rowNumber];
        Vector outputVector = output[i];
        Vector::copy(inputVector, outputVector);
    }
}

void Matrix::repeatRows(Vector &input)
{
    assert(input.elementCount == columnCount);
    
    for (size_t i = 0; i < rowCount; ++i) {

        Vector row = (*this)[i];
        Vector::copy(input, row);
    }
}

#ifdef _USE_HDF5_
void Matrix::save(std::string path, std::string dataset) const
{
    hsize_t datasetDimensions[2];
    datasetDimensions[0] = rowCount;
    datasetDimensions[1] = columnCount;
    
    saveData(path, dataset, datasetDimensions);
}

hid_t openScalarDataset(hid_t fileID, const char *dataSet, hsize_t dimensions[1])
{
    hid_t dataSetID, dataSpaceID;
    
    dataSetID = H5Dopen2(fileID, dataSet, H5P_DEFAULT);
    dataSpaceID =  H5Dget_space(dataSetID);
    H5Sget_simple_extent_dims(dataSpaceID, dimensions, NULL);
    
    H5Sclose(dataSpaceID);
    return dataSetID;
}


hid_t openMatrixDataset(hid_t fileID, const char *dataSet, hsize_t dimensions[2])
{
    hid_t dataSetID, dataSpaceID;
    
    dataSetID = H5Dopen2(fileID, dataSet, H5P_DEFAULT);
    dataSpaceID =  H5Dget_space(dataSetID);
    H5Sget_simple_extent_dims(dataSpaceID, dimensions, NULL);
    dimensions[0] = dimensions[0] == 0 ? 1 : dimensions[0];
    dimensions[1] = dimensions[1] == 0 ? 1 : dimensions[1];
    
    H5Sclose(dataSpaceID);
    return dataSetID;
}

SignalData::Matrix Matrix::dataSetToMatrix(const char *path, const char *dataSet, MYFLT **memory)
{
    herr_t status;
    
    hid_t fileID = 0;
    fileID = H5Fopen(path,
                     H5F_ACC_RDONLY,
                     H5P_DEFAULT);
    
    
    hsize_t dimensions[2] = {0};
    hid_t datasetID  = openMatrixDataset(fileID, dataSet, dimensions);
    
    *memory = (MYFLT *)calloc(dimensions[0] * dimensions[1], sizeof(MYFLT));
    
    status = H5Dread(datasetID, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, *memory);
    
    SignalData::Matrix matrix = SignalData::Matrix(**memory, dimensions[0], dimensions[1]);
    
    status = H5Dclose(datasetID);
    status = H5Fclose(fileID);
    
    return matrix;
}

MYFLT Matrix::dataSetToScalar(const char *path, const char *dataSet)
{
    herr_t status;
    
    hid_t fileID = 0;
    fileID = H5Fopen(path,
                     H5F_ACC_RDONLY,
                     H5P_DEFAULT);
    
    
    hsize_t dimensions[1] = {1};
    hid_t datasetID  = openScalarDataset(fileID, dataSet, dimensions);
    
    MYFLT scalar;
    
    status = H5Dread(datasetID, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, &scalar);
    
    
    status = H5Dclose(datasetID);
    status = H5Fclose(fileID);
    
    return scalar;
}


#endif

bool Matrix::equalDimensions(Matrix &inputA, Matrix &inputB)
{
    if (inputB.elementCount == inputA.elementCount
        &&
        inputB.rowCount == inputA.rowCount
        &&
        inputB.columnCount == inputA.columnCount) {
        
        return true;
    }
    else {
        
        return false;
    }
}

Matrix Matrix::subMatrix(size_t rowOffset, size_t rowCount)
{
    assert(rowOffset + rowCount <= this->rowCount);
    Matrix result(*(MYFLT *)&data[rowOffset * columnCount], rowCount, columnCount);
    
    return result;
}


void Matrix::resize(size_t rowCount, size_t columnCount)
{
    assert(rowCount * columnCount <= allocatedElementCount);
    memcpy((void *)&this->rowCount, (void *)&rowCount, sizeof(size_t));
    memcpy((void *)&this->columnCount, (void *)&columnCount, sizeof(size_t));
    size_t elementCount = rowCount * columnCount;
    memcpy((void *)&this->elementCount, (void *)&elementCount, sizeof(size_t));
}

#pragma mark - PVSegment -

PVSegment::PVSegment() :
magnitudes(),
frequencies(),
rowCount(0),
columnCount(0)
{
    
}

PVSegment::PVSegment(const Matrix &magnitudes, const Matrix &frequencies) :
magnitudes(magnitudes),
frequencies(frequencies),
rowCount(magnitudes.rowCount),
columnCount(magnitudes.columnCount)
{
    assert(magnitudes.rowCount == frequencies.rowCount);
    assert(magnitudes.columnCount == frequencies.columnCount);
}

PVSegment::PVSegment(ARRAYDAT *magnitudes, ARRAYDAT *frequencies) :
PVSegment(Matrix(magnitudes), Matrix(frequencies))
{
    
}

PVSegment::PVSegment(size_t rowCount, size_t columnCount, MYFLT *magnitudeMemory, MYFLT *frequencyMemory) :
magnitudes(Matrix(*magnitudeMemory, rowCount, columnCount)),
frequencies(Matrix(*frequencyMemory, rowCount, columnCount)),
rowCount(rowCount),
columnCount(columnCount)
{
    assert(this->magnitudes.rowCount == this->frequencies.rowCount);
    assert(this->magnitudes.columnCount == this->frequencies.columnCount);
}

PVSegment::PVSegment(csnd::Csound *csound, size_t rowCount, size_t columnCount) :
magnitudes(Matrix(csound, rowCount, columnCount)),
frequencies(Matrix(csound, rowCount, columnCount)),
rowCount(rowCount),
columnCount(columnCount)
{}

PVSegment PVSegment::operator=(PVSegment other)
{
    memcpy((void *)&magnitudes, &other.magnitudes, sizeof(Matrix));
    memcpy((void *)&frequencies, &other.frequencies, sizeof(Matrix));
    memcpy((void *)&rowCount, (void *)&other.rowCount, sizeof(size_t));
    memcpy((void *)&columnCount, (void *)&other.columnCount, sizeof(size_t));
    return *this;
}

PVFrame PVSegment::operator[](size_t index) const
{
    Vector mags = magnitudes[index];
    Vector freqs = frequencies[index];
    return PVFrame(mags, freqs);
}

void PVSegment::copyRows(PVSegment &input, Vector &specifiedRows, PVSegment &output)
{
    Matrix::copyRows((Matrix &)input.magnitudes, specifiedRows, (Matrix&)output.magnitudes);
    Matrix::copyRows((Matrix &)input.frequencies, specifiedRows, (Matrix&)output.frequencies);
}

PVSegment PVSegment::subSegment(size_t rowCount, size_t rowOffset)
{
    assert(rowOffset + rowCount <= this->rowCount);
    
    Matrix magnitudes(*(MYFLT *)&this->magnitudes.data[rowOffset * columnCount], rowCount, columnCount);
    Matrix frequencies(*(MYFLT *)&this->frequencies.data[rowOffset * columnCount], rowCount, columnCount);
    PVSegment result(magnitudes, frequencies);
    return result;
}

void PVSegment::copyColumns(PVSegment &input, size_t columnCount, size_t columnOffset, PVSegment &output)
{
    assert(output.columnCount == columnCount);
    assert(input.columnCount >= columnCount + columnOffset);
    
    Matrix::copyColumns(input.magnitudes, columnCount, columnOffset, output.magnitudes);
    Matrix::copyColumns(input.frequencies, columnCount, columnOffset, output.frequencies);
}

void PVSegment::copyColumns(PVSegment &input, size_t inputColumnCount, size_t inputColumnOffset,
                            PVSegment &output, size_t outputColumnOffset)
{
    
    Matrix::copyColumns(input.magnitudes, inputColumnCount, inputColumnOffset,
                        output.magnitudes, outputColumnOffset);
    Matrix::copyColumns(input.frequencies, inputColumnCount, inputColumnOffset,
                        output.frequencies, outputColumnOffset);
}

PVSegment PVSegment::interpolate(csnd::Csound *csound, PVSegment input, size_t rowCount)
{
    PVSegment output = PVSegment(csound, rowCount, input.columnCount);
    SignalMaths::linearInterpolateRows(input, output);
    return output;
}

void PVSegment::plot()
{
    magnitudes.plot();
}

#pragma mark - MatrixArray -

size_t MatrixArray::getMatrixArrayElementCount(Vector &rowCounts, Vector &columnCounts)
{
    assert(rowCounts.elementCount == columnCounts.elementCount);
    
    size_t elementCount = 0;
    
    for (size_t i = 0; i < rowCounts.elementCount; ++i) {
        
        elementCount += rowCounts[i] * columnCounts[i];
    }
    
    return elementCount;
}

MatrixArray::MatrixArray(MYFLT &matrixDataMem, Vector &rowCounts, Vector &columnCounts, Vector &offsets) :
Vector(matrixDataMem, (size_t)SignalMaths::dotProduct(rowCounts, columnCounts)),
rowCounts(rowCounts),
columnCounts(columnCounts),
offsets(offsets),
matrixCount(offsets.elementCount)
{
    assert(rowCounts.elementCount == columnCounts.elementCount
           &&
           rowCounts.elementCount == offsets.elementCount);
}

Matrix MatrixArray::operator[](size_t index)
{
    assert(index < elementCount);
    
    return Matrix((MYFLT &)data[(size_t)((Vector)offsets)[index]],
                  (size_t)((Vector)rowCounts)[index],
                  (size_t)((Vector)columnCounts)[index]);
}


MatrixArray &MatrixArray::operator=(MatrixArray other)
{
    
    memcpy((void *)&data, &other.data, sizeof(MYFLT *));
    memcpy((void *)&elementCount, (void *)&other.elementCount, sizeof(size_t));
    memcpy((void *)&matrixCount, (void *)&other.matrixCount, sizeof(size_t));
    memcpy((void *)&rowCounts, &other.rowCounts, sizeof(Vector));
    memcpy((void *)&columnCounts, &other.columnCounts, sizeof(Vector));
    memcpy((void *)&offsets, &other.offsets, sizeof(Vector));
    return *this;
}

#pragma mark - Cube -

Cube::Cube() :
Vector(),
sliceCount(0),
rowCount(0),
columnCount(0)
{}


Cube::Cube(MYFLT &data, size_t sliceCount, size_t rowCount, size_t columnCount) :
Vector(data, sliceCount * rowCount * columnCount),
sliceCount(sliceCount),
rowCount(rowCount),
columnCount(columnCount)
{}

Matrix Cube::operator[](size_t index)
{
    assert(index < sliceCount);
    return Matrix((MYFLT &)data[index * rowCount * columnCount], rowCount, columnCount);
}

Cube &Cube::operator=(Cube other)
{
    memcpy((void *)&data, &other.data, sizeof(MYFLT *));
    memcpy((void *)&elementCount, (void *)&other.elementCount, sizeof(size_t));
    memcpy((void *)&sliceCount, (void *)&other.sliceCount, sizeof(size_t));
    memcpy((void *)&rowCount, (void *)&other.rowCount, sizeof(size_t));
    memcpy((void *)&columnCount, (void *)&other.columnCount, sizeof(size_t));
    return *this;
}

void Cube::print()
{
    printf("Cube::print:\n");
    
    for (size_t i = 0; i < sliceCount; ++i) {
        
        for (size_t j = 0; j < rowCount; ++j) {
            
            for (size_t k = 0; k < columnCount; ++k) {
                
                printf("[%g]", (*this)[i][j][k]);
            }
            
            printf("\n");
        }
        
        printf("\n");
    }
    
    printf("\n");
}



