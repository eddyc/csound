//
//  UKFFormant.hpp
//  KalmanFormantOpcode
//
//  Created by Edward Costello on 27/08/2017.
//  Copyright © 2017 Edward Costello. All rights reserved.
//

#include <plugin.h>
#include "../common/json.hpp"
#include "../common/SignalData/Matrix.hpp"
#include "../common/SignalData/Vector.hpp"
#include "cJSON.h"

using namespace Signals;

static const char typeStrings[8][12] = {
    "STRING_VAR",
    "ARATE_VAR",
    "KRATE_VAR",
    "IRATE_VAR",
    "ARATE_ARRAY",
    "KRATE_ARRAY",
    "IRATE_ARRAY",
    "UNKNOWN"
};

typedef enum ArgumentType
{
    STRING_VAR,
    ARATE_VAR,
    KRATE_VAR,
    IRATE_VAR,
    ARATE_ARRAY,
    KRATE_ARRAY,
    IRATE_ARRAY,
    UNKNOWN
} ArgumentType;


typedef enum JsonType
{
    JSON_STRING,
    JSON_NUMBER,
    JSON_ARRAY,
    JSON_OBJECT,
    JSON_UNKNOWN
} JsonType;

typedef struct JsonArrayDimensions {
    
    size_t dimensions[2]; // No more than 3 dimensions please
    size_t dimensionsCount;
    size_t elements;
} JsonArrayDimensions;

struct ArgumentData
{
    ArgumentType argumentType;
    char *dataPath;
    JsonType jsonType;
    cJSON *jsonChild;
    MYFLT *outputArgument;
    Matrix<MYFLT> *matrix;
    Vector<MYFLT> *vector;
    JsonArrayDimensions arrayDims;
};

using namespace std;

class JsonInputOpcode : public csnd::Plugin <40, 40>  {
  

    ArgumentData *argumentData;
    
    CSOUND *cs;
    int inputArgumentCount;
    int outputArgumentCount;
    char *filePath;
    cJSON *fileJson;
    
    void initialiseArguments();
    void performArguments();
    void initialiseArgument(ArgumentData &argumentData);
    void performArgument(ArgumentData &argumentData);
    tuple<cJSON *, JsonType> getJsonChildForDataPath(char *dataPath);
    JsonArrayDimensions getJsonArrayDimensions(cJSON *child);

    void getFileJson();
    ArgumentType getArgumentTypeFromArgument(MYFLT *argument);
    void initArateVarArgument(ArgumentData &argData);
    void initArrayArgument(ArgumentData &argData);
    void performArateVarArgument(ArgumentData &argData);
    void performArrayArgument(ArgumentData &argData);


    char *getFilePath(MYFLT *argument);
    char *fileToString(const char *filePath);

public:
    int init();
    static int init_(CSOUND *csound, JsonInputOpcode *self) {
        self->init();
        return OK;
    }
    int kperf();
    static int kperf_(CSOUND *csound, JsonInputOpcode *self) {
        self->kperf();
        return OK;
    }
};




