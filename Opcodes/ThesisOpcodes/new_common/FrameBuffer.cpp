#include "FrameBuffer.hpp"
#include <Accelerate/Accelerate.h>

using namespace std;
template <typename T>
FrameBuffer<T>::FrameBuffer(const size_t inputSize,
                            const size_t hopSize,
                            const size_t windowSize,
                            const function<T*(size_t)> allocator,
                            const bool overlap)
    : inFrame(allocator, windowSize),
      outFrame(allocator, windowSize),
      inBuffer(allocator, windowSize),
      outBuffer(allocator, windowSize),
      inputSize(inputSize),
      hopSize(hopSize),
      windowSize(windowSize),
      overlap(overlap),
      slices((double)inputSize / (double)hopSize)
{
    size_t sizes[3] = {inputSize, hopSize, windowSize};
    sort(sizes, sizes + 3);

    for (int i = 0; i < 3; ++i) {
        if (sizes[i] % sizes[0] != 0 || (sizes[i] & (sizes[i] - 1)) != 0) {
            cout << "FrameBuffer, specified sizes not integer multiples, or "
                    "not powers of 2"
                 << endl;
            exit(-1);
        }
    }

    if (hopSize > windowSize) {
        cout << "FrameBuffer, hop size is bigger than window size, why?" << endl;
        exit(-1);
    }
}

template <typename T>
FrameBuffer<T>::~FrameBuffer()
{
}

template <typename T>
void FrameBuffer<T>::process(Vector<T>& input,
                             Vector<T>& output,
                             function<void(const Vector<T>& inFrame, const Vector<T>& outFrame)> callback)
{
    if (input.elementCount != output.elementCount || input.elementCount != inputSize) {
        cout << "FrameBuffer, inputs for process not sane" << endl;
        exit(-1);
    }

    // if (inputSize >= hopSize) {
    for (size_t i = 0; i < slices; ++i) {
        int vecSize = inputSize >= hopSize ? hopSize : input.elementCount;
        const auto inputSlice = input.sub(vecSize, i * vecSize);
        inBuffer.push(inputSlice);
        bufferedInputSamples += inputSize;
        bufferedInputSamples %= vecSize;
        if (bufferedInputSamples == 0) {
            outFrame.fill(0);
            Vector<T>::copy(inBuffer, inFrame);
            callback(inFrame, outFrame);
            inFrame.fill(0);

            if (overlap == true) {
                outBuffer.add(outFrame);
            }
            else {
                Vector<T>::copy(outFrame, outBuffer);
            }

            const auto temp = outBuffer.sub(vecSize, 0);
            Vector<T>::copy(temp, output.sub(vecSize, i * vecSize));
        }
        outBuffer.shift(-vecSize);
    }
}
// else {
//     cout << "FrameBuffer: input size less than or not equal to hopsize" <<
//     endl; exit(-1);
// }
// }
template class FrameBuffer<double>;
template class FrameBuffer<float>;
