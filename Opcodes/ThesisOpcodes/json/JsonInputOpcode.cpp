//
//  UKFFormant.cpp
//  Signals
//
//  Created by Edward Costello on 24/01/2018.
//  Copyright © 2018 Edward Costello. All rights reserved.
//

#include "JsonInputOpcode.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace std;

int JsonInputOpcode::init()
{

    cs = (CSOUND *)csound;
    inputArgumentCount = cs->GetInputArgCnt(this);
    outputArgumentCount = cs->GetOutputArgCnt(this);
    MYFLT *filePathArgument = outargs.data(outputArgumentCount);
    filePath = getFilePath(filePathArgument);

    getFileJson();
    initialiseArguments();

    return OK;
}

void JsonInputOpcode::getFileJson()
{
    char *buffer = fileToString(filePath);
    fileJson = cJSON_Parse(buffer);

    if (fileJson == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
    }
}

void JsonInputOpcode::initialiseArguments()
{
    if (inputArgumentCount - 1 != outputArgumentCount)
    {

        csound->init_error("Invalid number of specified input and output arguments");
    }

    argumentData = (ArgumentData *)csound->calloc(sizeof(ArgumentData) * outputArgumentCount);

    for (size_t i = 0; i < outputArgumentCount; ++i)
    {

        STRINGDAT *dataPathString = (STRINGDAT *)outargs.data(outputArgumentCount + 1 + i);
        argumentData[i].outputArgument = outargs.data(i);
        argumentData[i].dataPath = dataPathString->data;
        argumentData[i].argumentType = getArgumentTypeFromArgument(argumentData[i].outputArgument);
        auto [child, jsonType] = getJsonChildForDataPath(argumentData[i].dataPath);
        argumentData[i].jsonChild = child;
        argumentData[i].jsonType = jsonType;
        initialiseArgument(argumentData[i]);
    }
}

void JsonInputOpcode::initialiseArgument(ArgumentData &argumentData)
{
    switch (argumentData.argumentType)
    {
    case ARATE_VAR:
    {
        initArateVarArgument(argumentData);
        break;
    }
    case KRATE_ARRAY:
    {
        initArrayArgument(argumentData);
        break;
    }
    default:
        break;
    }
}

void JsonInputOpcode::performArgument(ArgumentData &argumentData)
{
    switch (argumentData.argumentType)
    {
    case ARATE_VAR:
    {
        performArateVarArgument(argumentData);
        break;
    }
    case KRATE_ARRAY:
    {
        performArrayArgument(argumentData);
        break;
    }
    case ARATE_ARRAY:
    {
        performArrayArgument(argumentData);
        break;
    }
    default:
        break;
    }
}

char *JsonInputOpcode::getFilePath(MYFLT *argument)
{
    ArgumentType type = getArgumentTypeFromArgument(argument);
    struct stat buffer;

    if (type != STRING_VAR)
    {
        csound->init_error("File argument must be a string.");
    }
    char *filePath = ((STRINGDAT *)argument)->data;

    if (stat(filePath, &buffer) != 0)
    {
        csound->init_error("File does not exist.");
    }
    return filePath;
}

JsonType checkJsonType(cJSON *object)
{
    JsonType jsonType = JSON_UNKNOWN;
    jsonType = cJSON_IsArray(object) ? JSON_ARRAY : JSON_UNKNOWN;
    jsonType = cJSON_IsString(object) ? JSON_STRING : jsonType;
    jsonType = cJSON_IsNumber(object) ? JSON_NUMBER : jsonType;
    jsonType = cJSON_IsObject(object) ? JSON_OBJECT : jsonType;
    return jsonType;
}

tuple<cJSON *, JsonType> JsonInputOpcode::getJsonChildForDataPath(char *dataPath)
{
    JsonType jsonType = JSON_UNKNOWN;
    int colons = 0;

    for (int i = 0; i < strlen(dataPath); ++i)
    {
        dataPath[i] == ':' ? colons++ : 0;
    }

    char *token = (char *)strtok(dataPath, ":");
    cJSON *child = fileJson;
    jsonType = checkJsonType(child);

    for (int i = 0; i <= colons && token != NULL; ++i)
    {

        switch (jsonType)
        {
        case JSON_ARRAY:
        {
            child = cJSON_GetArrayItem(child, atoi(token));
            break;
        }
        case JSON_OBJECT:
        {
            child = cJSON_GetObjectItemCaseSensitive(child, token);
            break;
        }
        default:
            break;
        }

        jsonType = checkJsonType(child);
        token = (char *)strtok(NULL, ":");
    }

    return std::make_tuple(child, jsonType);
}

int JsonInputOpcode::kperf()
{

    performArguments();
    return OK;
}

void JsonInputOpcode::performArguments()
{
    for (size_t i = 0; i < outputArgumentCount; ++i)
    {
        performArgument(argumentData[i]);
    }
}

void allocateArray(csnd::Csound *csound, size_t dimensions[], size_t dimensionsCount, ARRAYDAT *arrayOutArg)
{
    arrayOutArg->dimensions = (int)dimensionsCount;
    arrayOutArg->sizes = (int *)csound->calloc(sizeof(int) * dimensionsCount);
    arrayOutArg->sizes[0] = (int)dimensions[(size_t)0];

    size_t elementCount = dimensions[(size_t)0];

    if (dimensionsCount == 1 || dimensionsCount == 2)
    {
        size_t i;
        for (i = 1; i < dimensionsCount; ++i)
        {

            arrayOutArg->sizes[i] = (int)dimensions[i];
            elementCount *= arrayOutArg->sizes[i];
        }
    }
    else
    {
        csound->init_error("Error only 1 or 2 D dimensional arrays allowed");
    }

    CS_VARIABLE *arrayVariable = arrayOutArg->arrayType->createVariable(csound, NULL);
    arrayOutArg->arrayMemberSize = arrayVariable->memBlockSize;
    arrayOutArg->data = (MYFLT *)csound->calloc(sizeof(MYFLT) * elementCount);
}

JsonArrayDimensions JsonInputOpcode::getJsonArrayDimensions(cJSON *child)
{
    size_t arraySize = cJSON_GetArraySize(child);
    JsonArrayDimensions dimensions = {{0}};

    if (arraySize > 0)
    {

        dimensions.dimensionsCount = 1;
        dimensions.dimensions[0] = arraySize;
        dimensions.elements = arraySize;

        cJSON *arrayChild = cJSON_GetArrayItem(child, 0);
        JsonType childType = checkJsonType(arrayChild);

        while (childType == JSON_ARRAY)
        {

            dimensions.dimensionsCount++;
            dimensions.dimensions[dimensions.dimensionsCount - 1] = cJSON_GetArraySize(arrayChild);
            dimensions.elements *= dimensions.dimensions[dimensions.dimensionsCount - 1];
            arrayChild = cJSON_GetArrayItem(arrayChild, 0);
            childType = checkJsonType(arrayChild);

            if (dimensions.dimensionsCount > 2)
            {
                csound->init_error("Error only 1 or 2 dimensionsal arrays allowed");
            }
        }

        if (childType == JSON_NUMBER)
        {

            return dimensions;
        }
    }

    return {{0}};
}

void recursiveDataInit(cJSON *child, csnd::Csound *csound, MYFLT *data,
                       size_t dimensions[], size_t dimension,
                       size_t dimensionsCount, size_t *element)
{
    for (size_t i = 0; i < dimensions[dimension]; ++i)
    {

        if (dimension < dimensionsCount - 1)
        {
            cJSON *arrayChild = cJSON_GetArrayItem(child, i);
            recursiveDataInit(arrayChild, csound, data, dimensions,
                              dimension + 1, dimensionsCount, element);
        }
        else
        {

            cJSON *arrayChild = cJSON_GetArrayItem(child, i);
            data[*element] = arrayChild->valuedouble;
            *element += 1;
        }
    }
}

void JsonInputOpcode::initArateVarArgument(ArgumentData &argData)
{
    JsonArrayDimensions arrayDims = getJsonArrayDimensions(argData.jsonChild);
    if (arrayDims.dimensionsCount != 1)
    {
        csound->init_error("Arate var output requires 1 D arrays, exiting");
        return;
    }

    MYFLT *data = (MYFLT *)csound->calloc(sizeof(MYFLT) * arrayDims.dimensions[0]);
    argData.vector = new Vector<MYFLT>(data, arrayDims.dimensions[0]);

    for (size_t i = 0; i < argData.vector->elementCount; ++i)
    {

        cJSON *arrayChild = cJSON_GetArrayItem(argData.jsonChild, i);
        data[i] = arrayChild->valuedouble;
    }
}

void JsonInputOpcode::initArrayArgument(ArgumentData &argData)
{
    argData.arrayDims = getJsonArrayDimensions(argData.jsonChild);
    if (argData.arrayDims.dimensionsCount == 0)
    {
        csound->init_error("Array dimensions are 0, exiting");
        return;
    }

    if (argData.arrayDims.dimensionsCount == 2)
    {

        argData.matrix = new Matrix<MYFLT>((MYFLT *)csound->calloc(sizeof(MYFLT) * (size_t)argData.arrayDims.dimensions[0] * (size_t)argData.arrayDims.dimensions[1]), (size_t)argData.arrayDims.dimensions[0], (size_t)argData.arrayDims.dimensions[1]);

        size_t element = 0;
        recursiveDataInit(argData.jsonChild, csound,
                          (MYFLT *)argData.matrix->data,
                          argData.arrayDims.dimensions,
                          0, argData.arrayDims.dimensionsCount,
                          &element);
        ARRAYDAT *array = (ARRAYDAT *)argData.outputArgument;

        size_t secondDimension = 1;

        if (argData.argumentType == ARATE_ARRAY)
        {
            secondDimension = cs->GetKsmps(cs);
        }
        size_t dimensions[2] = {argData.arrayDims.dimensions[0], secondDimension};
        allocateArray(csound, dimensions, argData.arrayDims.dimensionsCount, array);
    }
}

void JsonInputOpcode::performArateVarArgument(ArgumentData &argData)
{
    int ksmps = cs->GetKsmps(cs);
    int kcount = cs->GetKcounter(cs) - 1;

    for (size_t i = 0; i < ksmps; ++i)
    {

        size_t index = ksmps * kcount + i;
        index %= argData.vector->elementCount;
        if (index < argData.vector->elementCount)
        {

            argData.outputArgument[i] = argData.vector->data[index];
        }
        else
        {

            //            argData.outputArgument[i] = 0;
            kcount = 0;
        }
    }
}

void JsonInputOpcode::performArrayArgument(ArgumentData &argData)
{
    size_t vectorSize = argData.argumentType == ARATE_ARRAY ? (size_t)cs->GetKsmps(cs) : argData.argumentType == KRATE_ARRAY ? 1 : 1;

    size_t counter = cs->GetKcounter(cs);
    if (argData.arrayDims.dimensionsCount == 2)
    {

        for (size_t i = 0; i < argData.arrayDims.dimensions[0] && counter * vectorSize < argData.arrayDims.dimensions[1]; ++i)
        {
            MYFLT *outputData = &((ARRAYDAT *)argData.outputArgument)->data[i * vectorSize];
            MYFLT *inputData = (MYFLT *)&argData.matrix->data[i * argData.matrix->columnCount + counter * vectorSize];

            memcpy(outputData, inputData, vectorSize * sizeof(MYFLT));
        }
    }
}

char *JsonInputOpcode::fileToString(const char *filePath)
{
    char *buffer = 0;
    long length;
    FILE *file = fopen((const char *)filePath, "rb");

    if (file)
    {

        fseek(file, 0, SEEK_END);
        length = ftell(file);
        fseek(file, 0, SEEK_SET);
        buffer = (char *)csound->calloc((length + 1) * sizeof(char));
        fread(buffer, 1, length, file);
        buffer[length] = '\0';
        fclose(file);
    }

    return buffer;
}

ArgumentType JsonInputOpcode::getArgumentTypeFromArgument(MYFLT *argument)
{
    CSOUND *cs = (CSOUND *)csound;
    const CS_TYPE *csoundType = cs->GetTypeForArg((void *)argument);
    const char *type = csoundType->varTypeName;
    ArgumentType argumentType = UNKNOWN;

    if (strcmp("S", type) == 0)
    {

        argumentType = STRING_VAR;
    }
    else if (strcmp("a", type) == 0)
    {

        argumentType = ARATE_VAR;
    }
    else if (strcmp("k", type) == 0)
    {

        argumentType = KRATE_VAR;
    }
    else if (strcmp("i", type) == 0)
    {

        argumentType = IRATE_VAR;
    }
    else if (strcmp("[", type) == 0)
    {

        ARRAYDAT *array = (ARRAYDAT *)argument;

        if (strcmp("k", array->arrayType->varTypeName) == 0)
        {

            argumentType = KRATE_ARRAY;
        }
        else if (strcmp("a", array->arrayType->varTypeName) == 0)
        {

            argumentType = ARATE_ARRAY;
        }
        else if (strcmp("i", array->arrayType->varTypeName) == 0)
        {

            argumentType = IRATE_ARRAY;
        }
    }

    return argumentType;
}
