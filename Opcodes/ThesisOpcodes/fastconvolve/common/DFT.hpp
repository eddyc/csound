#include "Vector.hpp"
#include <Accelerate/Accelerate.h>

#ifndef __DFT__
#define __DFT__
using namespace std;

typedef Vector<double> Vec;
class DFT {
    const size_t log2N, N;
    const FFTSetupD fftSetup;
    const DSPDoubleSplitComplex zInput;
    const Vec interlacedPolar;

public:
    DFT(function<double*(size_t)> allocator, size_t log2n);
    void realToPolar(const Vec& inputFrame, const Vec& outputMags, const Vec& outputPhases);
    void polarToReal(const Vec& inputMags, const Vec& inputPhases, const Vec& outputFrame);
};

#endif
