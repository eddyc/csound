#include "./NetCDFFile.hpp"

void checkStatus(int status, string error) {

    if (status != NC_NOERR) {

        cout << "Could not get " + error + ": Exiting" << endl;
        exit(-1);
    }
}

int getVarID(string name, int fileID) {
    int id = 0;
    int status = nc_inq_varid(fileID, name.c_str(), &id);
    checkStatus(status, "Could not get " + name + " ID: exiting");

    return id;
}

size_t getDimension(int fileID, string name) {

    int dimID = 0;
    int status = nc_inq_dimid(fileID, name.c_str(), &dimID);
    checkStatus(status, "Could not get " + name + " dimension ID: exiting");

    size_t dimLength = 0;
    status = nc_inq_dimlen(fileID, dimID, &dimLength);
    checkStatus(status, "Could not get " + name + " dimension Length: exiting");
    return dimLength;
}

NetCDFFile::NetCDFFile(string filePath, string type) {

    int status = nc_open(filePath.c_str(), NC_NOWRITE, &id);
    checkStatus(status, "input file");

    status = nc_inq(id, &dimensionCount, &variableCount, &attributeCount,
                    &unlimitedDimensionID);
    checkStatus(status, "file properties");

    M = getDimension(id, "M");
    R = getDimension(id, "R");
    N = getDimension(id, "N");
    C = getDimension(id, "C");
    I = getDimension(id, "I");

    const size_t startSize[3] = {0, 0, 0};
    const size_t endSize[3] = {M, R, N};

    if (type == "IR") {
        const size_t startSR = 0;
        const size_t countSR = 1;
        dataIRValues = vector<double>(N * R * M);

        status = nc_get_vara(id, getVarID("Data.SamplingRate", id), &startSR,
                             &countSR, &samplingRate);

        status = nc_get_vara(id, getVarID("Data.IR", id), startSize, endSize,
                             &dataIRValues[0]);

        checkStatus(status, "dataIRValues");
    } else if (type == "TF") {

        dataRealValues = vector<double>(N * R * M);
        dataImagValues = vector<double>(N * R * M);

        status = nc_get_vara(id, getVarID("Data.Real", id), startSize, endSize,
                             &dataRealValues[0]);

        checkStatus(status, "dataRealValues");
        status = nc_get_vara(id, getVarID("Data.Imag", id), startSize, endSize,
                             &dataImagValues[0]);

        checkStatus(status, "dataImagValues");
    }

    listener = getPositionVariable("Listener");

    source = getPositionVariable("Source");

    emitter = getPositionVariable("Emitter");

    sphere.position =
        vector<double>(source.positionLength[0] * source.positionLength[1]);
}

DataPoint NetCDFFile::getPositionVariable(string variableName) {

    DataPoint input = {0};

    int varID = getVarID(variableName + "Position", id);
    int status = nc_inq_var(id, varID, 0, &input.type, &input.dimensionCount,
                            &input.dimensionID[0], &input.attributeCount);
    checkStatus(status, variableName + "Position");

    for (int i = 0; i < input.dimensionCount; ++i) {

        status = nc_inq_dim(id, input.dimensionID[i], &input.dimensionName[i],
                            &input.positionLength[i]);
        checkStatus(status, variableName + " Dimension");
    }

    input.position =
        vector<double>(input.positionLength[0] * input.positionLength[1]);

    const size_t start[3] = {0};
    status = nc_get_vara(id, varID, start, &input.positionLength[0],
                         &input.position[0]);
    checkStatus(status, variableName + " Position");

    status = nc_inq_attlen(id, varID, "Type", &input.positionTypeLength);
    checkStatus(status, variableName + " Position Type Length");

    status = nc_inq_attlen(id, varID, "Units", &input.positionUnitsLength);
    checkStatus(status, variableName + " Position Units Length");

    status = nc_get_att(id, varID, "Type", &input.positionType);
    checkStatus(status, variableName + " Position Type");

    status = nc_get_att(id, varID, "Units", &input.positionUnits);

    checkStatus(status, variableName + " Position Units");
    cout << variableName << " Type: " << input.positionType << endl;
    cout << variableName << " Units: " << input.positionUnits << endl;
    return input;
}