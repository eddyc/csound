option(BUILD_FASTCONVOLVE_OPCODES "Build the fastconvolve opcodes" ON)
set(CMAKE_CXX_STANDARD 17)

if(BUILD_FASTCONVOLVE_OPCODES AND NOT EMSCRIPTEN)

  set(SOURCES
      common/Vector.hpp
      common/Vector.cpp
      common/FrameBuffer.hpp
      common/FrameBuffer.cpp
      common/DFT.hpp
      common/DFT.cpp
      common/Plot.hpp
      common/Plot.cpp
      common/DATFile.hpp
      common/DATFile.cpp
      common/NetCDFFile.hpp
      common/NetCDFFile.cpp
      module.cpp
      FastConvolve.cpp
      FastConvolve.hpp)

  find_library(ACCELERATE Accelerate)
  find_package(NetCDF)
  make_plugin(fastconvolve "${SOURCES}")
  include_directories(${ACCELERATE_H} ${NETCDF_INCLUDES})
  target_link_libraries(fastconvolve ${ACCELERATE} ${NETCDF_LIBRARIES})
endif()
