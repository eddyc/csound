//
//  DFT.c
//  ModVoc
//
//  Created by Edward Costello on 24/08/2014.
//  Copyright (c) 2014 Edward Costello. All rights reserved.
//

#include "DFT.hpp"

template <typename T>
DFT<T>::DFT(size_t FFTFrameSize, function<T *(size_t)> allocator) {}
template <typename T>
void DFT<T>::forward(Vector<T> &input, ComplexVector<T> &output) {}
template <typename T>
void DFT<T>::inverse(ComplexVector<T> &input, Vector<T> &output) {}
// FFT *FFT_new(size_t FFTFrameSize) {
//     FFT *self = calloc(1, sizeof(FFT));

//     self->FFTFrameSize = FFTFrameSize;
//     self->complexInputBuffer.realp = calloc(FFTFrameSize, sizeof(Float64));
//     self->complexInputBuffer.imagp = calloc(FFTFrameSize, sizeof(Float64));
//     self->complexOutputBuffer.realp = calloc(FFTFrameSize, sizeof(Float64));
//     self->complexOutputBuffer.imagp = calloc(FFTFrameSize, sizeof(Float64));
//     self->log2n = log2((Float64)FFTFrameSize);
//     self->fftSetup = vDSP_create_fftsetupD(self->log2n, FFT_RADIX2);

//     return self;
// }

// void FFT_delete(FFT *self) {
//     vDSP_destroy_fftsetupD(self->fftSetup);
//     free(self->complexInputBuffer.realp);
//     free(self->complexInputBuffer.imagp);
//     free(self->complexOutputBuffer.realp);
//     free(self->complexOutputBuffer.imagp);
//     free(self);
//     self = NULL;
// }

// void FFT_scopedDelete(FFT **self) { FFT_delete(*self); }

// void FFT_checkForwardInputMatrixSize(FFT *self, Matrix *input) {
//     if (input->elementCount != self->FFTFrameSize) {

//         printf("FFT_checkMatrixSize: Error, exiting\n");
//         exit(-1);
//     }
// }

// void FFT_checkForwardOutputMatrixSize(FFT *self, Matrix *input) {
//     if (input->elementCount != (self->FFTFrameSize / 2) + 1) {

//         printf("FFT_checkMatrixSize: Error, exiting\n");
//         exit(-1);
//     }
// }

// void FFT_checkInverseInputMatrixSize(FFT *self, Matrix *input) {
//     if (input->elementCount != (self->FFTFrameSize / 2) + 1) {

//         printf("FFT_checkMatrixSize: Error, exiting\n");
//         exit(-1);
//     }
// }

// void FFT_checkInverseOutputMatrixSize(FFT *self, Matrix *input) {
//     if (input->elementCount != self->FFTFrameSize) {

//         printf("FFT_checkMatrixSize: Error, exiting\n");
//         exit(-1);
//     }
// }

// void FFT_forwardReal(FFT *self, Matrix *input, Matrix *output) {
//     _Matrix_checkForVector(input);
//     _Matrix_checkForVector(output);
//     _FFT_checkForwardInputMatrixSize(self, input);
//     _FFT_checkForwardOutputMatrixSize(self, output);

//     vDSP_ctozD((DSPDoubleComplex *)input->data, 2, &self->complexInputBuffer,
//     1,
//                self->FFTFrameSize / 2);
//     vDSP_fft_zropD(self->fftSetup, &self->complexInputBuffer, 1,
//                    &self->complexOutputBuffer, 1, self->log2n,
//                    kFFTDirection_Forward);
//     Float64 pointFive = 0.5;

//     vDSP_vsmulD(self->complexOutputBuffer.realp, 1, &pointFive, output->data,
//     1,
//                 self->FFTFrameSize / 2);
//     vDSP_vsmulD(self->complexOutputBuffer.imagp, 1, &pointFive,
//                 output->imagData, 1, self->FFTFrameSize / 2);

//     output->data[self->FFTFrameSize / 2] = output->imagData[0];

//     vDSP_vnegD(output->imagData, 1, output->imagData, 1,
//                (self->FFTFrameSize / 2) + 1);
//     output->imagData[0] = 0;
//     Matrix_setComplex(output);
// }

// void FFT_inverseReal(FFT *self, Matrix *input, Matrix *output) {
//     _Matrix_checkForVector(input);
//     _Matrix_checkForVector(output);
//     _FFT_checkInverseInputMatrixSize(self, input);
//     _FFT_checkInverseOutputMatrixSize(self, output);

//     vDSP_vnegD(input->imagData, 1, input->imagData, 1,
//                (self->FFTFrameSize / 2) + 1);
//     input->imagData[0] = input->data[self->FFTFrameSize / 2];
//     input->data[self->FFTFrameSize / 2] = 0;

//     DSPDoubleSplitComplex splitComplex;
//     splitComplex.realp = input->data;
//     splitComplex.imagp = input->imagData;

//     vDSP_fft_zropD(self->fftSetup, &splitComplex, 1,
//     &self->complexOutputBuffer,
//                    1, self->log2n, kFFTDirection_Inverse);
//     Float64 scale = (Float64)1.0 / (self->FFTFrameSize);

//     vDSP_vsmulD(self->complexOutputBuffer.realp, 1, &scale,
//                 self->complexOutputBuffer.realp, 1, self->FFTFrameSize / 2);
//     vDSP_vsmulD(self->complexOutputBuffer.imagp, 1, &scale,
//                 self->complexOutputBuffer.imagp, 1, self->FFTFrameSize / 2);

//     vDSP_ztocD(&self->complexOutputBuffer, 1, (DSPDoubleComplex
//     *)output->data,
//                2, self->FFTFrameSize / 2);
// }

// void FFT_forwardComplex(FFT *self, Matrix *input, Matrix *output) {
//     _Matrix_checkDimensionEquality(2, input, output);

//     DSPDoubleSplitComplex fftInput = {.realp = input->realData,
//                                       .imagp = input->imagData};
//     DSPDoubleSplitComplex fftOutput = {.realp = output->realData,
//                                        .imagp = output->imagData};

//     vDSP_fft_zopD(self->fftSetup, &fftInput, 1, &fftOutput, 1, self->log2n,
//                   FFT_FORWARD);

//     Matrix_isComplex(output) = true;
// }

// void FFT_inverseComplex(FFT *self, Matrix *input, Matrix *output) {
//     _Matrix_checkDimensionEquality(2, input, output);

//     DSPDoubleSplitComplex fftInput = {.realp = input->realData,
//                                       .imagp = input->imagData};
//     DSPDoubleSplitComplex fftOutput = {.realp = output->realData,
//                                        .imagp = output->imagData};

//     vDSP_fft_zopD(self->fftSetup, &fftInput, 1, &fftOutput, 1, self->log2n,
//                   FFT_INVERSE);
//     Float64 scale = (Float64)1.0 / self->FFTFrameSize;

//     vDSP_vsmulD(output->realData, 1, &scale, output->realData, 1,
//                 self->FFTFrameSize);
//     vDSP_vsmulD(output->imagData, 1, &scale, output->imagData, 1,
//                 self->FFTFrameSize);

//     Matrix_isComplex(output) = true;
// }

// void FFT_forward(FFT *self, Matrix *input, Matrix *output) {
//     if (Matrix_isComplex(input) == true) {

//         FFT_forwardComplex(self, input, output);
//     } else {

//         FFT_forwardReal(self, input, output);
//     }
// }

// void FFT_inverse(FFT *self, Matrix *input, Matrix *output) {
//     if (Matrix_isComplex(input) == true) {

//         FFT_inverseComplex(self, input, output);
//     } else {

//         FFT_inverseReal(self, input, output);
//     }
// }
template class DFT<double>;
template class DFT<float>;
