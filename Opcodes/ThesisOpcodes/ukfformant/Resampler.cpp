//
//  Resampler.cpp
//  CsoundLib64
//
//  Created by Edward Costello on 23/08/2018.
//

#include "Resampler.hpp"

static size_t minSampleCount = 64;

Resampler::Resampler(CSOUND *cs, int vectorSize, int processVectorSize,
                     int inputChannelCount, int outputChannelCount, double ratio)
{
    if (vectorSize > minSampleCount) {
        
        minSampleCount = vectorSize;
    }
    
    downState = src_new(0, inputChannelCount, &errorDown);
    downData.data_in = (float *)cs->Calloc(cs, sizeof(float) * minSampleCount * inputChannelCount);
    downData.data_out = (float *)cs->Calloc(cs, sizeof(float) * minSampleCount * inputChannelCount);
    downData.input_frames = minSampleCount;
    downData.output_frames = minSampleCount;
    downData.src_ratio = ratio;
    
    upState = src_new(0, outputChannelCount, &errorUp);
    upData.data_in = (float *)cs->Calloc(cs, sizeof(float) * processVectorSize * outputChannelCount);
    upData.data_out = (float *)cs->Calloc(cs, sizeof(float) * minSampleCount * outputChannelCount);
    upData.input_frames = processVectorSize;
    upData.output_frames = minSampleCount;
    upData.src_ratio = 1.0 / ratio;
    
    new (&inputDeinterlace) Vecf((float *)cs->Calloc(cs, sizeof(float) * minSampleCount), minSampleCount);
    new (&outputDeinterlace) Vecf((float *)cs->Calloc(cs, sizeof(float) * minSampleCount), minSampleCount);
    new (&bufferedInput) Mat((double *)cs->Calloc(cs, sizeof(double) * minSampleCount * inputChannelCount), inputChannelCount, minSampleCount);
    new (&bufferedOutput) Mat((double *)cs->Calloc(cs, sizeof(double) * minSampleCount * outputChannelCount), outputChannelCount, minSampleCount);
    new (&outputf) Vecf((float *)cs->Calloc(cs, sizeof(float) * minSampleCount), minSampleCount);
    new (&processInput) Mat((double *)cs->Calloc(cs, sizeof(double) * processVectorSize * inputChannelCount), inputChannelCount, processVectorSize);
    new (&processOutput) Mat((double *)cs->Calloc(cs, sizeof(double) * processVectorSize * outputChannelCount), outputChannelCount, processVectorSize);
    new (&processInputf) Vecf((float *)cs->Calloc(cs, sizeof(float) * processVectorSize), processVectorSize);
    new (&processOutputf)Vecf((float *)cs->Calloc(cs, sizeof(float) * processVectorSize), processVectorSize);
    
    downRingBuffer = (RingBuffer<float> *)cs->Calloc(cs, sizeof(RingBuffer<float>) * inputChannelCount);
    upRingBuffer = (RingBuffer<float> *)cs->Calloc(cs, sizeof(RingBuffer<float>) * outputChannelCount);
    inputRingBuffer = (RingBuffer<double> *)cs->Calloc(cs, sizeof(RingBuffer<double>) * inputChannelCount);
    outputRingBuffer = (RingBuffer<double> *)cs->Calloc(cs, sizeof(RingBuffer<double>) * outputChannelCount);

    for (int i = 0; i < inputChannelCount; ++i ) {
        new (&downRingBuffer[i]) RingBuffer<float>(minSampleCount * 2);
        new (&inputRingBuffer[i]) RingBuffer<double>(minSampleCount);
    }
    
    double tempData[minSampleCount];
    Vec tempVec = Vec(tempData, minSampleCount).fill(0);
    
    for (int i = 0; i < outputChannelCount; ++i) {
        
        new (&upRingBuffer[i]) RingBuffer<float>(minSampleCount * 2);
        new (&outputRingBuffer[i]) RingBuffer<double>(minSampleCount);
        outputRingBuffer[i].write(tempData, minSampleCount);
    }
}

void Resampler::copyVectorToFloat(Vec input, float *output) {
    
    for (int i = 0; i < input.elementCount; ++i) {
        output[i] = input.data[i];
    }
}

void Resampler::copyVectorToFloat(Vec input, int stride, int offset, float *output) {
    
    for (int i = 0; i < input.elementCount; ++i) {
        output[i * stride + offset] = input.data[i];
    }
}

void Resampler::copyFloatToVector(float *input , Vec output) {
    
    for (int i = 0; i < output.elementCount; ++i) {
        double *data = (double *)output.data;
        data[i]= input[i];
    }
}

void destridedCopy(float *input, int length, int stride, int offset, float *output) {
    
    for (int i = 0; i < length; ++i) {
        
        output[i] = input[i * stride + offset];
    }
}

void stridedCopy(float *input, int length, int stride, int offset, float *output) {
    
    for (int i = 0; i < length; ++i) {
        
        output[i * stride + offset] = input[i];
    }
}


void Resampler::process(Mat &input, Mat &output, function<void(Mat &in, Mat &out)> resampledProcess)
{
    
    size_t availableInputSamples = 0;
    
    for (int i = 0; i < input.rowCount; ++i) {
        
        Vec inputVec = input[i];
        inputRingBuffer[i].write(inputVec.data, inputVec.elementCount);
        availableInputSamples = inputRingBuffer[i].getAvailableRead();
    }
    
    if (availableInputSamples == minSampleCount) {
        
        for (int i = 0; i < input.rowCount; ++i) {
            
            Vec bufferedInputVec = bufferedInput[i];
            inputRingBuffer[i].read((double *)bufferedInputVec.data, bufferedInputVec.elementCount);
        }
        
        resample(bufferedInput, bufferedOutput, resampledProcess);
        
        

        for (int i = 0; i < output.rowCount; ++i) {
            
            Vec bufferedOutputVec = bufferedOutput[i];
            outputRingBuffer[i].write(bufferedOutputVec.data, bufferedOutputVec.elementCount);
        }
    }
    
    for (int i = 0; i < output.rowCount; ++i) {
        
        Vec outputRow = output[i];
        
        outputRingBuffer[i].read((double *)outputRow.data, outputRow.elementCount);
    }
}


void Resampler::resample(Mat &input,
                         Mat &output,
                         function<void(Mat &in, Mat &out)> resampledProcess)
{
    size_t availableDownSamples = 0;
    
    for (int i = 0; i < input.rowCount; ++i) {
        
        Vec inputRow = input[i];
        copyVectorToFloat(inputRow, input.rowCount, i, (float *)downData.data_in);
    }
    
    src_process(downState, &downData);
    
    for (int i = 0; i < input.rowCount; ++i) {
        
        destridedCopy(downData.data_out, downData.output_frames_gen, input.rowCount, i, (float *)inputDeinterlace.data);
        downRingBuffer[i].write(inputDeinterlace.data, downData.output_frames_gen);
        
        availableDownSamples = downRingBuffer[i].getAvailableRead();
    }
    
    if (availableDownSamples >= processInput.columnCount) {
        
        for (size_t i = 0; i < availableDownSamples - processInput.columnCount; i += processInput.columnCount) {
            
            for (int j = 0; j < input.rowCount; ++j) {
                
                downRingBuffer[j].read((float *)processInputf.data, processInputf.elementCount);
                Vec processInputVec = processInput[j];
                Vec::toDouble(processInputf, processInputVec);
            }
            
            resampledProcess(processInput, processOutput);
            
            for (int j = 0; j < output.rowCount; ++j) {
                
                Vec::toFloat(processOutput[j], processOutputf);
                stridedCopy((float *)processOutputf.data, processOutputf.elementCount,
                            output.rowCount, j, (float *)upData.data_in);
            }
            
            src_process(upState, &upData);
            
            for (int j = 0; j < output.rowCount; ++j) {
                
                destridedCopy((float *)upData.data_out, upData.output_frames_gen, output.rowCount, j, (float *)outputDeinterlace.data);
                upRingBuffer[j].write((float *)outputDeinterlace.data, upData.output_frames_gen);
            }
        }
    }
    
    size_t availableUpSamples = 0;
    
    for (int i = 0; i < output.rowCount; ++i) {
        
        availableUpSamples = upRingBuffer[i].getAvailableRead();
        if (availableUpSamples >= input.columnCount) {
            
            upRingBuffer[i].read((float *)outputf.data, outputf.elementCount);
            Vec outputRow = output[i];
            Vec::toDouble(outputf, outputRow);
        }
        else {
            output.fill(0);
        }
    }
}

