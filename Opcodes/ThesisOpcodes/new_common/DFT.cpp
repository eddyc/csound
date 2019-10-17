#include "DFT.hpp"

DFT::DFT(function<double*(size_t)> allocator, size_t log2nIn)
    : log2N(log2nIn),
      N(pow(2, log2nIn)),
      fftSetup(vDSP_create_fftsetupD(log2nIn, kFFTRadix2)),
      zInput{(double*)allocator(N / 2), (double*)allocator(N / 2)},
      interlacedPolar(Vec(allocator(N + 2), N + 2))
{
}

void DFT::realToPolar(const Vec& inputFrame, const Vec& outputMags, const Vec& outputPhases)
{
    if (outputMags.elementCount != outputPhases.elementCount ||
        inputFrame.elementCount != N || outputMags.elementCount != N / 2 + 1) {
        cout << "DFT::realToPolar inputs not sane" << endl;
        exit(-1);
    }
    vDSP_ctozD((DSPDoubleComplex*)inputFrame.data, 2, &zInput, 1, N / 2);
    vDSP_fft_zripD(fftSetup, &zInput, 1, log2N, kFFTDirection_Forward);
    zInput.realp[N / 2] = zInput.imagp[0];
    zInput.imagp[N / 2] = 0;
    zInput.imagp[0] = 0;

    vDSP_ztocD(&zInput, 1, (DSPDoubleComplex*)interlacedPolar.data, 2, N / 2 + 1);
    vDSP_polarD(interlacedPolar.data, 2, (double*)interlacedPolar.data, 2, N / 2 + 1);

    cblas_dcopy((u_int32_t)N / 2 + 1, &interlacedPolar.data[0], 2,
                (double*)outputMags.data, 1);
    cblas_dcopy((u_int32_t)N / 2 + 1, &interlacedPolar.data[1], 2,
                (double*)outputPhases.data, 1);
}

void DFT::realToComplex(const Vec& inputFrame, const Vec& realOut, const Vec& imagOut)
{
    if (realOut.elementCount != imagOut.elementCount ||
        inputFrame.elementCount != N || realOut.elementCount != N / 2 + 1) {
        cout << "DFT::realToPolar inputs not sane" << endl;
        exit(-1);
    }

    DSPDoubleSplitComplex zOutput = {.realp = realOut.data, .imagp = imagOut.data};
    vDSP_ctozD((DSPDoubleComplex*)inputFrame.data, 2, &zInput, 1, N / 2);
    vDSP_fft_zropD(fftSetup, &zInput, 1, &zOutput, 1, log2N, kFFTDirection_Forward);
    zOutput.realp[N / 2] = zInput.imagp[0];
    zOutput.imagp[N / 2] = 0;
    zOutput.imagp[0] = 0;
}

void DFT::polarToReal(const Vec& inMags, const Vec& inPhases, const Vec& outputFrame)
{
    if (inMags.elementCount != inPhases.elementCount ||
        outputFrame.elementCount != N || inMags.elementCount != N / 2 + 1) {
        cout << "DFT::polarToReal inputs not sane" << endl;
        exit(-1);
    }
    cblas_dcopy((u_int32_t)N / 2 + 1, inMags.data, 1,
                (double*)&interlacedPolar.data[0], 2);
    cblas_dcopy((u_int32_t)N / 2 + 1, inPhases.data, 1,
                (double*)&interlacedPolar.data[1], 2);

    vDSP_rectD(interlacedPolar.data, 2, (double*)interlacedPolar.data, 2, N / 2 + 1);
    vDSP_ctozD((DSPDoubleComplex*)interlacedPolar.data, 2, &zInput, 1, N / 2 + 1);

    zInput.imagp[0] = zInput.realp[N / 2];
    zInput.realp[N / 2] = 0;
    zInput.imagp[N / 2] = 0;

    vDSP_fft_zripD(fftSetup, &zInput, 1, log2N, kFFTDirection_Inverse);
    vDSP_ztocD(&zInput, 1, (DSPDoubleComplex*)outputFrame.data, 2, N / 2);

    outputFrame.multiply(0.5 / N);
}

void DFT::inPlaceForwardComplex(const Vec& real, const Vec& imag)
{
    DSPDoubleSplitComplex zIn = {.realp = (double*)real.data,
                                 .imagp = (double*)imag.data};
    vDSP_fft_zipD(fftSetup, &zIn, 1, log2N, kFFTDirection_Forward);
}
