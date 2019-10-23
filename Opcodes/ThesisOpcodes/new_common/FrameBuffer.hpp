#include "Vector.hpp"
#include <functional>
#include <iostream>
using namespace std;

template <typename T>
class FrameBuffer
{
private:
    const Vector<T> inFrame, outFrame, inBuffer, outBuffer;
    const size_t inputSize, hopSize, windowSize;
    const bool overlap;
    size_t bufferedInputSamples = 0;
    const double slices;

public:
    FrameBuffer(const size_t inputSize,
                const size_t hopSize,
                const size_t windowSize,
                const function<T*(size_t)> allocator,
                const bool overlap);
    ~FrameBuffer();
    void process(Vector<T>& input,
                 Vector<T>& output,
                 function<void(const Vector<T>& inFrame, const Vector<T>& outFrame)> callback);
};
