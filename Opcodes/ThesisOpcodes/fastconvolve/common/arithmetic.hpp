
#include <Accelerate/Accelerate.h>
#include <cstdlib>
// #if __APPLE__
// #define _fill fillAccelerate
// #include <Accelerate/Accelerate.h>
// #else
// #define _fill fillStandard
// #endif

void fillAccelerate(double* data, size_t length, double value);
void fillAccelerate(float* data, size_t length, float value);
