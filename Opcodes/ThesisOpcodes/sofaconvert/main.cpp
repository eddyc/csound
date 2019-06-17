
#include "../common/FrameBuffer.hpp"
#include "../common/SignalData/Plotter.hpp"
#include <csound.h>
#include <netcdf.h>
using namespace Signals;
using namespace std;
typedef Vector<double> Vec;

class SOFAFile {

  public:
    int dimensionCount;
    int variableCount;
    int attributeCount;
    int unlimitedDimensionsID;
};

int main(int argc, char const *argv[]) {

    function<tuple<double *, size_t>(size_t)> allocator =
        [&](size_t count) -> tuple<double *, size_t> {
        return {(double *)(calloc(count, sizeof(double))), count};
    };

    SOFAFile sofaFile;
    string filename = "/Users/eddyc/Desktop/EvaluationGrid.sofa";
    int fileID;
    int status = nc_open(filename.c_str(), NC_NOWRITE, &fileID);

    if (status != NC_NOERR) {

        cout << "File not opened" << endl;
        return -1;
    }

    status = nc_inq(fileID, &sofaFile.dimensionCount, &sofaFile.variableCount,
                    &sofaFile.attributeCount, &sofaFile.unlimitedDimensionsID);
    Vec test = Vec(allocator(5));

    test.print();
    return 0;
}
