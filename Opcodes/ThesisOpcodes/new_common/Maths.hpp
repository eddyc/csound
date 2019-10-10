#if defined(__APPLE__)
#include <Accelerate/Accelerate.h>
const auto vadd = vDSP_vadd;
const auto vaddD = vDSP_vaddD;
#endif