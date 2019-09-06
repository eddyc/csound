#include "Vector.hpp"
#include <Accelerate/Accelerate.h>

using namespace std;
using namespace Signals;

typedef Vector<double> Vec;
typedef Matrix<double> Mat;
class DFT {
    const size_t log2N, N;
    const FFTSetupD fftSetup;
    const DSPDoubleSplitComplex zInput;
    const Vec interlacedPolar;

public:
    DFT(function<double*(size_t)> allocator, size_t log2n);
    void realToPolar(Vec& inputFrame, Vec& outputMags, Vec& outputPhases);
    void polarToReal(Vec& inputMags, Vec& inputPhases, Vec& outputFrame);
};
