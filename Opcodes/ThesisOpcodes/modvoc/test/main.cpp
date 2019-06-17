
#include "../../common/FrameBuffer.hpp"
#include "../../common/SignalData/Plotter.hpp"
#include <csound.h>
using namespace Signals;
using namespace std;
typedef Vector<double> Vec;
void runCsound() {

    csoundInitialize(0);

    CSOUND *csound = csoundCreate(NULL);
    const char *args[2];
    args[0] = "csound";
    args[1] = "/Volumes/Data/GoogleDrive/PhD/Thesis/Software/csound/Opcodes/"
              "ThesisOpcodes/modvoc/test/test.csd";

    csoundCompile(csound, 2, args);
    csoundPerform(csound);
    csoundStop(csound);
}

void runFrameBuffer() {
    const size_t dataSize = 64;
    const size_t windowSize = 8;
    const size_t hopSize = 2;
    const size_t streamSize = 1;

    auto allocator = [&](size_t count) -> double * {
        return (double *)calloc(count, sizeof(double));
    };

    FrameBuffer<double> fb(streamSize, windowSize, hopSize, allocator);
    double data[dataSize] = {0};
    double windowData[windowSize] = {0.5, 0.5, 1, 1, 1, 1, 0.5, 0.5};
    Vec input = Vec(data, dataSize).fill(1);
    Vec output = Vec(data, dataSize);
    Vec window = Vec(windowData, windowSize);
    input.print();
    window.print();

    for (int i = 0; i < input.elementCount; ++i) {

        Vec in = Vec(&input[i], 1);
        Vec out = Vec(&output[i], 1);

        fb.process(in, out, [&](Vec &inputFrame, Vec &outputFrame) {
            // inputFrame.print();
            Vec::copy(inputFrame, outputFrame);

            Vec::multiply(outputFrame, window, outputFrame);
            outputFrame.multiply(1. / 3.);
        });

        out.print();
    }
}

int main(int argc, char const *argv[]) {

    runCsound();

    return 0;
}
