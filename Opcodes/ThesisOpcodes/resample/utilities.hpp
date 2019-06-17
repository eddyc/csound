//
//  UKFRigollRecursion.hpp
//  CsoundLib64
//
//  Created by Edward Costello on 21/03/2018.
//

#ifndef utilities_hpp
#define utilities_hpp
#include "../common/SignalData/Matrix.hpp"
#include "../common/SignalData/Vector.hpp"
#include <plugin.h>
#include <hdf5.h>

using namespace Signals;

string readFile(const string &fileName);
void allocateArray(csnd::Csound *csound, size_t dimensions[], size_t dimensionsCount, ARRAYDAT *arrayOutArg);
tuple<double *, size_t> allocateVector(size_t elementCount);
tuple<double *, size_t, size_t> allocateMatrix(size_t rowCount, size_t columnCount);
void saveData(std::string path, std::string dataset, hsize_t datasetDimensions[2], const double *data);
bool ispowerof2(unsigned int x);
#endif /* utilities_hpp */
