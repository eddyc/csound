#include "../new_common/Vector.hpp"
#include <Accelerate/Accelerate.h>
#include <plugin.h>
#ifndef __CENTREOFGRAVITY__
#define __CENTREOFGRAVITY__
using namespace std;

template <typename T>
class CentreOfGravity
{
    T logSpectralBinsCount;
    T step;
    T erbStep;
    ResizableVector<T> peaktag, pidx_offs, pidx_offs_prev, weights, pidx_diff;
    Vector<T> peak_idx, peak_idx_prev, proximity, segment, index_offs, tens;
    bool firstFrame;

    void getWeights(Vector<T> logYPsd, ResizableVector<T> peakidx, ResizableVector<T> weights);

public:
    T maximumPeakCount;
    T halfStep;
    CentreOfGravity(function<T*(size_t)> allocator, size_t Fs, size_t binCount);
    void operator()(const Vector<T>&, const ResizableVector<T>&, const ResizableVector<T>&);
};

#endif
