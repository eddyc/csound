#include "../new_common/Detrender.hpp"
#include "../new_common/Interpolator.hpp"
#include "../new_common/Vector.hpp"
#include <Accelerate/Accelerate.h>
#include <plugin.h>
#ifndef __LOGYPSD__
#define __LOGYPSD__
using namespace std;

template <typename T>
class LogYPsd
{
    struct FilterArgument {
        Vector<T> currentState, previousState;
        FilterArgument(function<T*(size_t)> allocator, size_t frameSize);
    };

    const Vector<T> ramp, interpScale, magnitudes;
    Vector<T> interpOut, detrended, tens;
    const FilterArgument nom, denom;
    Interpolator<T> interpolator;
    Detrender<T> detrender;
    double alpha;

public:
    LogYPsd(function<T*(size_t)> allocator, size_t Fs, size_t binCount, size_t hopSize);
    void operator()(const Vector<T>&, const Vector<T>&, const Vector<T>&);
    void filter(const FilterArgument& arg, const T alpha);
};

#endif
