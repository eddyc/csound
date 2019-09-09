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
      outBuffer(allocator, windowSize),
      inputSize(inputSize),
      hopSize(hopSize),
      windowSize(windowSize),
      overlap(overlap)
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

    const double slices = inputSize / hopSize;
    if (inputSize > hopSize) {
        for (size_t i = 0; i < slices; ++i) {
            const auto inputSlice = input.sub(hopSize, i * hopSize);
            inFrame.push(inputSlice);
            bufferedInputSamples += inputSize;
            bufferedInputSamples %= hopSize;
            if (bufferedInputSamples == 0) {
                callback(inFrame, outFrame);

                if (overlap == true) {
                    outBuffer.add(outFrame);
                }
                else {
                    Vector<T>::copy(outFrame, outBuffer);
                }

                const auto temp = outBuffer.sub(hopSize, 0);
                Vector<T>::copy(temp, output.sub(hopSize, i * hopSize));
            }
            outBuffer.shift(-hopSize);
        }
    }
}
template class FrameBuffer<double>;
template class FrameBuffer<float>;
