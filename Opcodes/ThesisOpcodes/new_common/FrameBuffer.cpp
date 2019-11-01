#include "FrameBuffer.hpp"

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

    if (inputSize >= hopSize) {
        for (size_t i = 0; i < slices; ++i) {
            const auto inputSlice = input.sub(hopSize, i * hopSize);
            inBuffer.push(inputSlice);
            bufferedInputSamples += inputSize;
            bufferedInputSamples %= hopSize;
            if (bufferedInputSamples == 0) {
                outFrame.fill(0);
                Vector<T>::copy(inBuffer, inFrame);
                callback(inFrame, outFrame);
                inFrame.fill(0);

                if (overlap == true) {
                    const double overlapMult = windowSize / inputSize;
                    outFrame.multiply(1. / overlapMult);
                    outBuffer.add(outFrame);
                } else {
                    Vector<T>::copy(outFrame, outBuffer);
                }

                const auto temp = outBuffer.sub(hopSize, 0);
                Vector<T>::copy(temp, output.sub(hopSize, i * hopSize));
            }
            outBuffer.shift(-hopSize);
        }
    } else {
        inBuffer.push(input);
        bufferedInputSamples += inputSize;
        bufferedInputSamples %= hopSize;
        if (bufferedInputSamples == 0) {
            outFrame.fill(0);
            Vector<T>::copy(inBuffer, inFrame);
            callback(inFrame, outFrame);
            inFrame.fill(0);

            if (overlap == true) {
                const double overlapMult = windowSize / hopSize;
                outFrame.multiply(1. / overlapMult);
                outBuffer.add(outFrame);
            } else {
                Vector<T>::copy(outFrame, outBuffer);
            }
        }
        const auto temp = outBuffer.sub(inputSize, 0);
        Vector<T>::copy(temp, output);
        outBuffer.shift(-inputSize);
    }
}
template class FrameBuffer<double>;
template class FrameBuffer<float>;
