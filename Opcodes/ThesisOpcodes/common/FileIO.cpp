//
//  FileIO.cpp
//  Signals
//
//  Created by Edward Costello on 14/01/2018.
//  Copyright © 2018 Edward Costello. All rights reserved.
//

#include "FileIO.hpp"
#include <hdf5.h>
using namespace Signals;

void FileIO::openDataset(string filePath, string datasetName,
                         function<tuple<double *, size_t, size_t>(size_t rowCount, size_t columnCount)> allocator,
                         function<void(double *data, size_t rowCount, size_t columnCount)> assigner)
{
    hid_t fileID, dataSetID, dataSpaceID;
    hsize_t dimensions[2] = {0};
    herr_t status;
    
    fileID = H5Fopen(filePath.c_str(),
                     H5F_ACC_RDONLY,
                     H5P_DEFAULT);
    
    dataSetID = H5Dopen2(fileID, datasetName.c_str(), H5P_DEFAULT);
    dataSpaceID =  H5Dget_space(dataSetID);
    
    H5Sget_simple_extent_dims(dataSpaceID, dimensions, NULL);
    
    tuple<double *, size_t, size_t> p = allocator(dimensions[0], dimensions[1]);
    
    status = H5Dread(dataSetID, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, (void *)get<0>(p));
    
    status = H5Sclose(dataSpaceID);
    status = H5Dclose(dataSetID);
    status = H5Fclose(fileID);
    
    assigner(get<0>(p), dimensions[0], dimensions[1]);
}

