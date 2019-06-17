#include "FrameBuffer.hpp"

template <typename T>
FrameBuffer<T>::FrameBuffer(size_t streamSize, size_t frameSize, size_t hopSize,
                            function<T *(size_t)> allocator)

    : inputBuffer(frameSize), outputBuffer(frameSize),
      callbackInputFrame(allocator(frameSize), frameSize),
      callbackOutputFrame(allocator(frameSize), frameSize),
      olaFrame(allocator(frameSize), frameSize),
      hopSilence(allocator(hopSize), hopSize), streamSize(streamSize),
      frameSize(frameSize), hopSize(hopSize) {

    outputBuffer.write(callbackInputFrame.data, frameSize);
    inputBuffer.write(callbackInputFrame.data, frameSize);
    inputBuffer.consume(hopSize);
}

template <typename T>
void FrameBuffer<T>::process(
    Vector<T> &input, Vector<T> &output,
    function<void(Vector<T> &inputFrame, Vector<T> &outputFrame)> callback) {

    inputBuffer.write(input.data, input.elementCount);
    outputBuffer.peak((T *)(output.data), output.elementCount);
    outputBuffer.consume(streamSize);

    const size_t availableWrite = inputBuffer.getAvailableWrite();
    if (availableWrite == 0) {

        inputBuffer.peak((T *)(callbackInputFrame.data),
                         callbackInputFrame.elementCount);
        callback(callbackInputFrame, callbackOutputFrame);

        outputBuffer.write((T *)(hopSilence.data), hopSize);
        outputBuffer.peak((T *)(olaFrame.data), olaFrame.elementCount);
        outputBuffer.clear();
        Vector<T>::add(callbackOutputFrame, olaFrame, callbackOutputFrame);

        outputBuffer.write(callbackOutputFrame.data,
                           callbackOutputFrame.elementCount);
        outputBuffer.peak((T *)(olaFrame.data), olaFrame.elementCount);

        inputBuffer.consume(hopSize);
    }
}

template class FrameBuffer<double>;
template class FrameBuffer<float>;