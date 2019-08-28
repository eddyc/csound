//
//  UKFRigollRecursion.cpp
//  CsoundLib64
//
//  Created by Edward Costello on 21/03/2018.
//

#include "utilities.hpp"
#include <iostream>
#include <math.h>
#include <string>
#include <fstream>
#include <vector>

string readFile(const string &fileName)
{
    ifstream ifs(fileName.c_str(), ios::in | ios::binary | ios::ate);
    
    ifstream::pos_type fileSize = ifs.tellg();
    ifs.seekg(0, ios::beg);
    
    vector<char> bytes(fileSize);
    ifs.read(bytes.data(), fileSize);
    
    return string(bytes.data(), fileSize);
}

void allocateArray(csnd::Csound *csound, size_t dimensions[], size_t dimensionsCount, ARRAYDAT *arrayOutArg)
{
    arrayOutArg->dimensions = (int)dimensionsCount;
    arrayOutArg->sizes = (int *)csound->calloc(sizeof(int) * dimensionsCount);
    arrayOutArg->sizes[0] = (int)dimensions[(size_t)0];
    
    size_t elementCount = dimensions[(size_t)0];
    
    if (dimensionsCount == 1 || dimensionsCount == 2)
    {
        size_t i;
        for (i = 1; i < dimensionsCount; ++i)
        {
            
            arrayOutArg->sizes[i] = (int)dimensions[i];
            elementCount *= arrayOutArg->sizes[i];
        }
    }
    else
    {
        csound->init_error("Error only 1 or 2 D dimensional arrays allowed");
    }
    
    CS_VARIABLE *arrayVariable = arrayOutArg->arrayType->createVariable(csound, NULL);
    arrayOutArg->arrayMemberSize = arrayVariable->memBlockSize;
    arrayOutArg->data = (MYFLT *)csound->calloc(sizeof(MYFLT) * elementCount);
}

tuple<double *, size_t> allocateVector(size_t elementCount)  {
    return {(double *)calloc(elementCount, sizeof(double)),
        elementCount};
}

tuple<double *, size_t, size_t> allocateMatrix(size_t rowCount, size_t columnCount) {
    return {
        (double *)calloc(rowCount * columnCount, sizeof(double)),
        rowCount,
        columnCount};
}

bool ispowerof2(unsigned int x)
{
    return x && !(x & (x - 1));
}

void saveData(std::string path, std::string dataset, hsize_t datasetDimensions[2], const double *data)
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
    
    hid_t attributeID  = H5Screate(H5S_SCALAR);
    hid_t attributeType = H5Tcopy(H5T_C_S1);
    H5Tset_size(attributeType, 11);
    H5Tset_strpad(attributeType,H5T_STR_NULLTERM);
    hid_t attributeHandle = H5Acreate2(dataSetID,
                                       ".", attributeType, attributeID,
                                       H5P_DEFAULT, H5P_DEFAULT);
    H5Awrite(attributeHandle, attributeType, "KRATE_ARRAY");
    H5Sclose(attributeID);
    H5Tclose(attributeType);
    H5Aclose(attributeHandle);
    
    
    status = H5Dclose(dataSetID);
    
    status = H5Sclose(dataSpaceID);
    status = H5Fclose(fileID);
}

