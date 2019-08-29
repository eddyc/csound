//
//  Plot.h
//  ModulationVocoder
//
//  Created by Edward Costello on 28/07/2014.
//  Copyright (c) 2014 Edward Costello. All rights reserved.
//

#include "RingBuffer.hpp"
#include "SignalData/Matrix.hpp"
#include <MacTypes.h>
#include <functional>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace Signals;
using namespace std;
template <typename T>
class FrameBuffer {
private:
    RingBuffer<T> inputBuffer, outputBuffer;
    Vector<T> callbackInputFrame, callbackOutputFrame, olaFrame, hopSilence;
    size_t streamSize, frameSize, hopSize;

public:
    FrameBuffer(size_t streamSize, size_t frameSize, size_t hopSize, function<T*(size_t)> allocator);

public:
    void process(Vector<T>& input,
                 Vector<T>& output,
                 function<void(Vector<T>& inputFrame, Vector<T>& outputFrame)> callback);
};
