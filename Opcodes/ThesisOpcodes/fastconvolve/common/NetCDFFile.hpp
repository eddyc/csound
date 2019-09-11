#include <iostream>
#include <math.h>
#include <netcdf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <vector>

using namespace std;

struct DataPoint {

    int type;
    int dimensionCount;
    int dimensionID[3];
    char dimensionName[3];
    size_t dimensionLength[3];
    size_t positionLength[3];
    size_t positionTypeLength;
    size_t positionUnitsLength;
    char positionType[100];
    char positionUnits[100];

    vector<double> position;
    int attributeCount;
};

class NetCDFFile {
  public:
    int id;
    int dimensionCount;
    int attributeCount;
    int variableCount;
    int unlimitedDimensionID;
    double samplingRate;

    size_t M;
    size_t R;
    size_t N;
    size_t C;
    size_t I;
    vector<double> dataIRValues;
    vector<double> dataRealValues;
    vector<double> dataImagValues;
    nc_type sourceType;

    DataPoint listener;
    DataPoint source;
    DataPoint emitter;
    DataPoint sphere;
    NetCDFFile(string filePath, string type);
    DataPoint getPositionVariable(string name);
};
