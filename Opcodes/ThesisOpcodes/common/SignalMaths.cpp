//
//  MatrixMaths.cpp
//  DTW
//
//  Created by Edward Costello on 16/02/2017.
//  Copyright © 2017 Edward Costello. All rights reserved.
//

#include "SignalMaths.hpp"
#include <assert.h>
using namespace SignalData;

#ifdef _USE_ACCELERATE_
#include <Accelerate/Accelerate.h>


void SignalMaths::add(const Vector &input, const MYFLT scalar) {
    vDSP_vsaddD((const MYFLT *)input.data, 1, (const MYFLT *)&scalar,
                (MYFLT *)input.data, 1, input.elementCount);
}

void SignalMaths::add(const Vector &inputA, const Vector &inputB,
                      const Vector &output) {
    assert(inputA.elementCount == inputB.elementCount);
    assert(output.elementCount == inputB.elementCount);
    
    vDSP_vaddD(inputA.data, inputA.stride,
               inputB.data, inputB.stride,
               (MYFLT *)output.data, output.stride,
               inputA.elementCount);
}

const Vector &SignalMaths::multiply(const Vector &input, const MYFLT scalar) {
    vDSP_vsmulD((const MYFLT *)input.data, input.stride, (const MYFLT *)&scalar,
                (MYFLT *)input.data, input.stride, input.elementCount);
    return input;
}

const Vector &SignalMaths::divide(const Vector &input, const MYFLT scalar) {
    vDSP_vsdivD((const MYFLT *)input.data, 1, (const MYFLT *)&scalar,
                (MYFLT *)input.data, 1, input.elementCount);
    return input;
}

void SignalMaths::substract(const Vector &inputA, const Vector &inputB,
                            const Vector &output) {
    assert(inputA.elementCount == inputB.elementCount);
    assert(output.elementCount == inputB.elementCount);
    
    vDSP_vsubD(inputA.data, 1, inputB.data, 1, (MYFLT *)output.data, 1,
               inputA.elementCount);
}

void SignalMaths::substract(const Vector &inputA, const MYFLT inputB,
                            const Vector &output) {
    assert(inputA.elementCount == output.elementCount);
    MYFLT scalar = -inputB;
    
    vDSP_vsaddD((const MYFLT *)inputA.data, 1, (const MYFLT *)&scalar,
                (MYFLT *)output.data, 1, output.elementCount);
}

void SignalMaths::square(const Vector &input, const Vector &output) {
    assert(input.elementCount == output.elementCount);
    
    vDSP_vsqD(input.data, 1, (MYFLT *)output.data, 1, input.elementCount);
}

MYFLT SignalMaths::sum(const Vector &input) {
    MYFLT sum = 0;
    vDSP_sveD(input.data, 1, &sum, input.elementCount);
    return sum;
}

void SignalMaths::fill(const Vector &input, MYFLT value) {
    vDSP_vfillD(&value, (MYFLT *)input.data, 1, input.elementCount);
}

void SignalMaths::fillRow(const Matrix &input, size_t row, MYFLT value) {
    assert(row < input.rowCount);
    vDSP_vfillD(&value, (MYFLT *)&input.data[row * input.columnCount], 1,
                input.columnCount);
}

void SignalMaths::fillColumn(const Matrix &input, size_t column, MYFLT value) {
    assert(column < input.columnCount);
    vDSP_vfillD(&value, (MYFLT *)&input.data[column], input.columnCount,
                input.rowCount);
}

std::pair<size_t, MYFLT> SignalMaths::minimum(Vector &input) {
    size_t index;
    MYFLT value;
    
    vDSP_minviD(input.data, 1, &value, &index, input.elementCount);
    return std::pair<size_t, MYFLT>{index, value};
}

void SignalMaths::reverse(const Vector &input) {
    vDSP_vrvrsD((MYFLT *)input.data, 1, input.elementCount);
}

MYFLT SignalMaths::dotProduct(const Vector &inputA, const Vector &inputB) {
    
    assert(inputA.elementCount == inputB.elementCount);
    
    MYFLT output = 0;
    vDSP_dotprD(inputA.data, 1, inputB.data, 1, &output, inputA.elementCount);
    return output;
}

void SignalMaths::transpose(const Matrix &inputA, const Matrix &inputB) {
    assert(inputA.columnCount == inputB.rowCount);
    assert(inputB.columnCount == inputA.rowCount);
    assert(inputA.elementCount == inputB.elementCount);
    
    vDSP_mtransD((const MYFLT *)inputA.data, 1, (MYFLT *)inputB.data, 1,
                 inputA.columnCount, inputA.rowCount);
}

void SignalMaths::ramp(const Vector &input, const MYFLT initial,
                       const MYFLT delta) {
    vDSP_vrampD(&initial, &delta, (MYFLT *)input.data, 1, input.elementCount);
}



const Matrix &SignalMaths::identity(const Matrix &input) {
    assert(input.columnCount == input.rowCount);
    assert(input.stride == 1);
    MYFLT one = 1;
    vDSP_vfillD(&one, (MYFLT *)input.data, (size_t)input.columnCount + 1,
                (size_t)input.columnCount);
    
    return input;
}


const Vector &SignalMaths::clear(const Vector &input) {
    vDSP_vclrD((MYFLT *)&input.data, 1, input.elementCount);
    return input;
}




#else

void SignalMaths::add(const Vector &input, const MYFLT scalar) {
    
    MYFLT *inputData = (MYFLT *)input.data;
    for (size_t i = 0; i < input.elementCount; ++i) {
        inputData[i * input.stride] += scalar;
    }
}

void SignalMaths::add(const Vector &inputA, const Vector &inputB,
                      const Vector &output) {
    assert(inputA.elementCount == inputB.elementCount);
    assert(output.elementCount == inputB.elementCount);
    
    MYFLT *outputData = (MYFLT *)output.data;
    for (size_t i = 0; i < inputA.elementCount; ++i) {
        outputData[i * output.stride] = inputA.data[i * inputA.stride] + inputB.data[i * inputB.stride];
    }
}

void SignalMaths::multiply(const Vector &inputA, const Vector &inputB,
                           const Vector &output) {
    assert(inputA.elementCount == inputB.elementCount);
    assert(output.elementCount == inputB.elementCount);
    
    MYFLT *outputData = (MYFLT *)output.data;
    for (size_t i = 0; i < inputA.elementCount; ++i) {
        outputData[i * output.stride] = inputA.data[i * inputA.stride] * inputB.data[i * inputB.stride];
    }
}

const Vector &SignalMaths::multiply(const Vector &input, const MYFLT scalar) {
    
    MYFLT *inputData = (MYFLT *)input.data;
    for (size_t i = 0; i < input.elementCount; ++i) {
        inputData[i * input.stride] *= scalar;
    }
    
    return input;
}


void SignalMaths::multiply(const Vector &input, const MYFLT scalar, Vector &output)
{
    assert(input.elementCount == output.elementCount);
    MYFLT *inputData = (MYFLT *)input.data;
    for (size_t i = 0; i < input.elementCount; ++i) {
        output[i * output.stride] = inputData[i * input.stride] * scalar;
    }
}

void SignalMaths::divide(const Vector &input, const MYFLT scalar, Vector &output)
{
    assert(input.elementCount == output.elementCount);
    MYFLT *inputData = (MYFLT *)input.data;
    for (size_t i = 0; i < input.elementCount; ++i) {
        output[i * output.stride] = inputData[i * input.stride] / scalar;
    }
}

void SignalMaths::multiply(const Matrix &inputA, const Matrix &inputB, const Matrix &output)
{
    assert(inputA.columnCount == inputB.rowCount);
    assert(output.rowCount == inputA.rowCount);
    assert(output.columnCount == inputB.columnCount);
    
    for (size_t i = 0; i < inputA.rowCount; i++) {
        
        for (size_t j = 0; j < inputB.columnCount; j++) {
            
            output[i][j] = 0;
            
            for (size_t k = 0; k < inputA.columnCount; k++){
                
                output[i][j] += inputA[i][k] * inputB[k][j];
            }
        }
    }
}

const Vector &SignalMaths::divide(const Vector &input, const MYFLT scalar) {
    MYFLT *inputData = (MYFLT *)input.data;
    for (size_t i = 0; i < input.elementCount; ++i) {
        inputData[i * input.stride] /= scalar;
    }
    
    return input;
}

void SignalMaths::substract(const Vector &inputA, const Vector &inputB,
                            const Vector &output) {
    assert(inputA.elementCount == inputB.elementCount);
    assert(output.elementCount == inputB.elementCount);
    
    MYFLT *outputData = (MYFLT *)output.data;
    for (size_t i = 0; i < inputA.elementCount; ++i) {
        outputData[i * output.stride] = inputA.data[i * inputA.stride] - inputB.data[i * inputB.stride];
    }
}

void SignalMaths::substract(const Vector &inputA, const MYFLT inputB,
                            const Vector &output) {
    assert(inputA.elementCount == output.elementCount);
    
    MYFLT *outputData = (MYFLT *)output.data;
    for (size_t i = 0; i < inputA.elementCount; ++i) {
        outputData[i * output.stride] = inputA.data[i * inputA.stride] - inputB;
    }
}

void SignalMaths::square(const Vector &input, const Vector &output) {
    assert(input.elementCount == output.elementCount);
    
    MYFLT *outputData = (MYFLT *)output.data;
    for (size_t i = 0; i < input.elementCount; ++i) {
        outputData[i * output.stride] = input.data[i * input.stride] * input.data[i * input.stride];
    }
}

MYFLT SignalMaths::sum(const Vector &input) {
    MYFLT sum = 0;
    for (size_t i = 0; i < input.elementCount; ++i) {
        sum += input.data[i * input.stride];
    }
    return sum;
}

void SignalMaths::fill(const Vector &input, MYFLT value) {
    
    MYFLT *inputData = (MYFLT *)input.data;
    for (size_t i = 0; i < input.elementCount; ++i) {
        inputData[i * input.stride] = value;
    }
}

void SignalMaths::fillRow(const Matrix &input, size_t row, MYFLT value) {
    assert(row < input.rowCount);
    printf("NOT IMPLEMENTED\n\n\nEXITING\n\n");
    exit(-1);
}

void SignalMaths::fillColumn(const Matrix &input, size_t column, MYFLT value) {
    assert(column < input.columnCount);
    printf("NOT IMPLEMENTED\n\n\nEXITING\n\n");
    exit(-1);
}

std::pair<size_t, MYFLT> SignalMaths::minimum(Vector &input) {
//    size_t index;
//    MYFLT value;
    printf("NOT IMPLEMENTED\n\n\nEXITING\n\n");
    exit(-1);
}

void SignalMaths::reverse(const Vector &input) {
    
    for (size_t i = 0; i < input.elementCount / 2; ++i) {
        
        MYFLT *data = (MYFLT *)input.data;
        MYFLT x = data[i];
        data[i] = data[input.elementCount - 1 - i];
        data[input.elementCount - 1 - i] = x;
    }
}

MYFLT SignalMaths::dotProduct(const Vector &inputA, const Vector &inputB) {
    
    assert(inputA.elementCount == inputB.elementCount);
    MYFLT output = 0;
    
    for (size_t i = 0; i < inputA.elementCount; ++i) {
        
        output += inputA.data[i] * inputB.data[i];
    }
    
    return output;
}

void SignalMaths::transpose(const Matrix &inputA, const Matrix &inputB) {
    assert(inputA.columnCount == inputB.rowCount);
    assert(inputB.columnCount == inputA.rowCount);
    assert(inputA.elementCount == inputB.elementCount);
    
    MYFLT *dataA = (MYFLT *)inputA.data;
    MYFLT *dataB = (MYFLT *)inputB.data;
    
    for(size_t i = 0; i < inputA.rowCount; ++i) {
        
        for(size_t j = 0; j < inputA.columnCount; ++j) {
            
            dataB[j * inputA.rowCount + i] = dataA[i * inputA.columnCount + j];
        }
    }
}

void SignalMaths::ramp(const Vector &input, const MYFLT initial,
                       const MYFLT delta) {
    printf("NOT IMPLEMENTED\n\n\nEXITING\n\n");
    exit(-1);
}



const Matrix &SignalMaths::identity(const Matrix &input) {
    assert(input.columnCount == input.rowCount);
    clear(input);
    for (size_t i = 0; i < input.columnCount; ++i) {
        input[i][i] = 1;
    }
    
    return input;
}

const Vector &SignalMaths::clear(const Vector &input) {
    MYFLT *inputData = (MYFLT *)input.data;
    for (size_t i = 0; i < input.elementCount; ++i) {
        inputData[i * input.stride] = 0;
    }
    return input;
}


void SignalMaths::exponent(const Vector &input) {
    MYFLT *inputData = (MYFLT *)input.data;
    for (size_t i = 0; i < input.elementCount; ++i) {
        inputData[i * input.stride] = exp(inputData[i * input.stride]);
    }
}

void SignalMaths::cosine(const Vector &input) {
    MYFLT *inputData = (MYFLT *)input.data;
    for (size_t i = 0; i < input.elementCount; ++i) {
        inputData[i * input.stride] = cos(inputData[i * input.stride]);
    }
}


#endif


void SignalMaths::square(const Vector &input) {
    square(input, input);
}

void SignalMaths::linearInterpolateRows(PVSegment &input, PVSegment &output) {
    linearInterpolateRows((Matrix &)input.magnitudes,
                          (Matrix &)output.magnitudes);
    linearInterpolateRows((Matrix &)input.frequencies,
                          (Matrix &)output.frequencies);
}

void SignalMaths::linearInterpolateRows(Matrix &input, Matrix &output) {
    assert(input.columnCount == output.columnCount);
    
    if (input.rowCount == output.rowCount) {
        
        Vector::copy((Matrix &)input, (Matrix &)output);
        return;
    }
    
    for (size_t i = 0; i < output.rowCount; ++i) {
        
        MYFLT index = (MYFLT)i / (((MYFLT)output.rowCount - 1));
        index *= input.rowCount - 1;
        
        MYFLT modulus = fmod(index, 1);
        Vector outputRow = output[i];
        
        if (modulus > 0) {
            
            size_t startIndex = index - modulus;
            size_t endIndex = startIndex + 1;
            
            Vector startVector = input[startIndex];
            Vector endVector = input[endIndex];
            
            SignalMaths::substract(startVector, endVector, outputRow);
            SignalMaths::multiply(outputRow, modulus);
            SignalMaths::add(startVector, outputRow, outputRow);
        } else {
            
            Vector inputRow = input[index];
            Vector::copy(inputRow, outputRow);
        }
    }
}

void SignalMaths::transpose(const PVSegment &inputA, const PVSegment &inputB) {
    transpose(inputA.magnitudes, inputB.magnitudes);
    transpose(inputA.frequencies, inputB.frequencies);
}

const Matrix &SignalMaths::identity(const Matrix &input, MYFLT diagonal) {
    
    identity(input);
    Vector diag = input.diagonal();
    multiply(diag, diagonal);
    return input;
}

void _divide(MYFLT *data, MYFLT scalar, size_t stride, size_t elementCount) {
    
    for (size_t i = 0; i < elementCount; ++i) {
        data[i * stride] /= scalar;
    }
}

void SignalMaths::choleskyFactorisation(const Matrix &input) {
    
    for (size_t i = 0; i < input.rowCount; ++i) {
        
        input[i][i] = sqrt(input[i][i]);
//        Matrix_getRow(input, i)[i] = sqrt(Matrix_getRow(input, i)[i]);
        input[i].subVector(i + 1, input.rowCount - i - 1).clear();
//        vDSP_vclrD(&Matrix_getRow(input, i)[i + 1], 1, input->rowCount - i - 1);
        MYFLT *data = (MYFLT *)input.data;
        
        _divide((MYFLT *)&data[((i + 1) * input.columnCount) + i], input[i][i], input.columnCount, (size_t)(input.rowCount - i - 1));
//        vDSP_vsdivD(&Matrix_getRow(input, i + 1)[i], input->columnCount, &Matrix_getRow(input, i)[i], &Matrix_getRow(input, i + 1)[i], input->columnCount, input->rowCount - i - 1);
//
        for (size_t j = i + 1; j < input.rowCount; ++j) {
//
            for (size_t k = 0; k < input.rowCount - i - 1; ++k) {
//
//                Float64 scalar = Matrix_getRow(input, j)[i];
                MYFLT scalar = input[j][i];
                Vector vec = input[i + 1];
//                scalar *= Matrix_getRow(input, i + 1)[i + k * input->columnCount];
                MYFLT *data = (MYFLT *)vec.data;
                scalar *= data[i + k * input.columnCount];
                data[j + k * input.columnCount] -= scalar;
//                Matrix_getRow(input, i + 1)[j + k * input->columnCount] -= scalar;
            }
        }
    }
}

void SignalMaths::sumRows(const Matrix &input, Vector &output)
{
    assert(input.rowCount == output.elementCount);
    
    for (size_t i = 0; i < input.rowCount; ++i) {

        output[i] = SignalMaths::sum(input[i]);
    }
}
