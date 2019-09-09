#include "Vector.hpp"
#include <Accelerate/Accelerate.h>

#ifndef __DATFile__
#define __DATFile__
using namespace std;

typedef Vector<double> Vec;
typedef Matrix<double> Mat;
class DATFile {
public:
    const size_t rowCount;
    const size_t columnCount;
    Mat magnitudes, phases;

    DATFile(function<double*(size_t)> allocator, string path);
};

#endif
