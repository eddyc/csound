option(BUILD_DYNAMICWARPING_OPCODES "Build the dynamicwarping opcodes" OFF)
option(BUILD_UKFFORMANT_OPCODES "Build the ukfformant opcodes" OFF)
option(BUILD_MODVOC_OPCODE "Build the modulation vocoder opcode" ON)
option(BUILD_FASTCONVOLVE_OPCODES "Build the fastconvolve opcode" OFF)
set(__USE_ACCELERATE__ 1)
if(BUILD_SOFA_CONVERT AND NOT EMSCRIPTEN)
  set(CS_LIB CsoundLib64)

  set(SOURCES
      common/RingBuffer.hpp
      common/RingBuffer.cpp
      common/FrameBuffer.hpp
      common/FrameBuffer.cpp
      common/SignalData/DFT.hpp
      common/SignalData/DFT.cpp
      common/SignalData/Plotter.hpp
      common/SignalData/Plotter.cpp
      common/SignalData/Matrix.hpp
      common/SignalData/Matrix.cpp
      common/SignalData/MatrixMaths.hpp
      common/SignalData/MatrixMathsAccelerate.cpp
      common/SignalData/Vector.hpp
      common/SignalData/Vector.cpp
      common/SignalData/VectorMaths.hpp
      common/SignalData/VectorMathsAccelerate.cpp
      common/SignalData/StridedVectorMaths.hpp
      common/SignalData/StridedVectorMathsAccelerate.cpp
      sofaconvert/main.cpp)
  set(CMAKE_CXX_STANDARD 17)
  find_package(HDF5)
  find_package(netCDF)
  find_library(ACCELERATE Accelerate)

  find_library(CS_LIB_PATH ${CS_LIB})
  find_path(CSOUND_H_PATH csound.h)
  include_directories(${CSOUND_H_PATH})

  add_executable(sofaconvert "${SOURCES}")
  target_link_libraries(sofaconvert
                        ${CS_LIB_PATH}
                        ${HDF5_LIBRARIES}
                        ${ACCELERATE}
                        ${NETCDF_LIBRARIES})
  target_include_directories(sofaconvert
                             PRIVATE ${HDF5_INCLUDE_DIRS}
                                     ${ACCELERATE_H}
                                     ${NETCDF_INCLUDES}
                                     sofaconvert
                                     common)
endif()

if(BUILD_MODVOC_OPCODE AND NOT EMSCRIPTEN)

  set(SOURCES
      modvoc/module.cpp
      modvoc/ModVoc.cpp
      modvoc/ModVoc.hpp
      modvoc/LogYPsd.hpp
      modvoc/LogYPsd.cpp
      new_common/Maths.hpp
      new_common/Maths.cpp
      new_common/FrameBuffer.hpp
      new_common/FrameBuffer.cpp
      new_common/Interpolator.hpp
      new_common/Interpolator.cpp
      new_common/DFT.hpp
      new_common/DFT.cpp
      new_common/Plot.hpp
      new_common/Plot.cpp
      new_common/Vector.hpp
      new_common/Vector.cpp)

  find_package(HDF5)
  check_deps(BUILD_MODVOC_OPCODE HDF5_FOUND)
  if(BUILD_MODVOC_OPCODE)
    message(STATUS "HDF5_INCLUDE_DIRS: ${HDF5_INCLUDE_DIRS}")
    message(STATUS "HDF5_LIBRARIES: ${HDF5_LIBRARIES}")

    set(CMAKE_CXX_STANDARD 17)

    if(APPLE)

      find_library(ACCELERATE Accelerate)
      make_plugin(modvoc "${SOURCES}")
      include_directories()
      target_include_directories(
        modvoc
        PRIVATE
          ${HDF5_INCLUDE_DIRS}
          ${ACCELERATE_H}
          /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/System/Library/Frameworks/Accelerate.framework/Versions/Current/Frameworks/vecLib.framework/Headers/
          modvoc
          common)
      target_link_libraries(modvoc
                            ${HDF5_LIBRARIES}
                            ${ACCELERATE}
                            ${LIBSNDFILE_LIBRARY})

    else()

      make_plugin(modvoc "${SOURCES}")
      target_include_directories(modvoc
                                 PRIVATE ${HDF5_INCLUDE_DIRS} modvoc common)
      target_link_libraries(modvoc ${HDF5_LIBRARIES})
    endif()

  endif()
endif()

if(BUILD_DYNAMICWARPING_OPCODES AND NOT EMSCRIPTEN)

  set(SOURCES
      dynamicwarping/module.cpp
      dynamicwarping/FramedDW.cpp
      dynamicwarping/DynamicWarping.cpp
      dynamicwarping/DifferenceMatrix.cpp
      dynamicwarping/BasicDW.cpp
      dynamicwarping/FramedDW.hpp
      dynamicwarping/DynamicWarping.hpp
      dynamicwarping/DifferenceMatrix.hpp
      dynamicwarping/BasicDW.hpp
      dynamicwarping/module.cpp
      common/SignalData.cpp
      common/SignalMaths.cpp
      common/Plot.cpp
      common/SignalData.hpp
      common/SignalMaths.hpp
      common/Plot.hpp
      common/CsoundUtility.hpp
      common/CsoundUtility.cpp)

  find_package(HDF5)
  find_library(ACCELERATE Accelerate)
  check_deps(BUILD_DYNAMICWARPING_OPCODES HDF5_FOUND)
  if(BUILD_DYNAMICWARPING_OPCODES)
    message(STATUS "HDF5_INCLUDE_DIRS: ${HDF5_INCLUDE_DIRS}")
    message(STATUS "HDF5_LIBRARIES: ${HDF5_LIBRARIES}")

    set(CMAKE_CXX_STANDARD 11)
    make_plugin(dynamicwarping "${SOURCES}")
    target_include_directories(dynamicwarping
                               PRIVATE ${HDF5_INCLUDE_DIRS}
                                       ${ACCELERATE_H}
                                       dynamicwarping
                                       common)
    target_link_libraries(dynamicwarping ${HDF5_LIBRARIES} ${ACCELERATE})
  endif()
endif()
check_include_file(hdf5.h HDF5_H)

if(BUILD_UKFFORMANT_OPCODES AND NOT EMSCRIPTEN)

  set(SOURCES
      ukfformant/UKFFormant.hpp
      ukfformant/UKFFormant.cpp
      ukfformant/KalmanFilter.hpp
      ukfformant/KalmanFilter.cpp
      ukfformant/Resampler.hpp
      ukfformant/Resampler.cpp
      ukfformant/UKFRigollRecursion.cpp
      ukfformant/UKFRigollRecursion.hpp
      ukfformant/ScaledSymmetricSigmaPoints.cpp
      ukfformant/ScaledSymmetricSigmaPoints.hpp
      ukfformant/utilities.cpp
      ukfformant/utilities.hpp
      ukfformant/RingBuffer.cpp
      ukfformant/RingBuffer.hpp
      ukfformant/module.cpp
      common/SignalData/Matrix.hpp
      common/SignalData/Matrix.cpp
      common/SignalData/MatrixMaths.hpp
      common/SignalData/MatrixMathsAccelerate.cpp
      common/SignalData/Vector.hpp
      common/SignalData/Vector.cpp
      common/SignalData/VectorMaths.hpp
      common/SignalData/VectorMathsAccelerate.cpp
      common/SignalData/StridedVectorMaths.hpp
      common/SignalData/StridedVectorMathsAccelerate.cpp
      common/json.hpp)

  find_library(ACCELERATE Accelerate)
  find_library(LIBSAMPLERATE samplerate)
  find_path(LIBSAMPLERATE_INCLUDE_DIR samplerate.h)

  if(BUILD_UKFFORMANT_OPCODES)

    set(CMAKE_CXX_STANDARD 17)
    make_plugin(ukfformant "${SOURCES}")
    add_library(LAPACK STATIC IMPORTED)
    add_library(BLAS STATIC IMPORTED)
    add_library(F2C STATIC IMPORTED)

    set_target_properties(LAPACK
                          PROPERTIES IMPORTED_LOCATION
                                     /usr/local/lib/liblapack.a)
    set_target_properties(BLAS
                          PROPERTIES IMPORTED_LOCATION /usr/local/lib/libblas.a)
    set_target_properties(F2C
                          PROPERTIES IMPORTED_LOCATION /usr/local/lib/libf2c.a)

    message(STATUS "LAPACK_LIBRARIES: ${LAPACK}")

    include_directories(/usr/local/include ${ACCELERATE_H})
    target_link_libraries(ukfformant
                          F2C
                          BLAS
                          LAPACK
                          ${ACCELERATE}
                          ${HDF5_LIBRARIES}
                          ${LIBSAMPLERATE})
  endif()
endif()

if(BUILD_FASTCONVOLVE_OPCODES AND NOT EMSCRIPTEN)

  set(SOURCES
      new_common/Vector.hpp
      new_common/Vector.cpp
      new_common/FrameBuffer.hpp
      new_common/FrameBuffer.cpp
      new_common/DFT.hpp
      new_common/DFT.cpp
      new_common/Plot.hpp
      new_common/Plot.cpp
      new_common/DATFile.hpp
      new_common/DATFile.cpp
      new_common/NetCDFFile.hpp
      new_common/NetCDFFile.cpp
      fastconvolve/module.cpp
      fastconvolve/FastConvolve.cpp
      fastconvolve/FastConvolve.hpp)

  find_library(ACCELERATE Accelerate)
  find_path(
    cblas_PATH cblas.h
    PATHS
      /System/Library/Frameworks/Accelerate.framework/Versions/Current/Frameworks/vecLib.framework/Headers/
    )
  mark_as_advanced(ACCELERATE)
  find_package(NetCDF)
  make_plugin(fastconvolve "${SOURCES}")
  include_directories(${ACCELERATE_H} ${NETCDF_INCLUDES} ${cblas_PATH})
  target_link_libraries(fastconvolve
                        ${ACCELERATE}
                        ${NETCDF_LIBRARIES}
                        ${BLAS})
endif()

check_include_file(hdf5.h HDF5_H)
