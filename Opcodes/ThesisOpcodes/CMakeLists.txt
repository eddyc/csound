option(BUILD_DYNAMICWARPING_OPCODES "Build the dynamicwarping opcodes" ON)
option(BUILD_UKFFORMANT_OPCODES "Build the ukfformant opcodes" ON)
option(BUILD_JSON_OPCODES "Build the json opcodes" ON)
option(BUILD_RESAMPLE_OPCODES "Build the resample opcode" ON)
option(BUILD_TESTOPCODE "Build the testopcode" ON)
option(BUILD_MODVOC_OPCODE "Build the modulation vocoder opcode" ON)
option(BUILD_MODVOC_TEST "Build modvoc test" ON)
option(BUILD_SOFA_CONVERT "Build sofa convert" ON)

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
    sofaconvert/main.cpp
)
set (CMAKE_CXX_STANDARD 17)
find_package(HDF5)
find_package(NetCDF)
find_library(ACCELERATE Accelerate)

    find_library(CS_LIB_PATH ${CS_LIB})
    find_path(CSOUND_H_PATH csound.h)
    include_directories(${CSOUND_H_PATH})
  
    add_executable(sofaconvert "${SOURCES}")
    target_link_libraries(sofaconvert ${CS_LIB_PATH} ${HDF5_LIBRARIES} ${ACCELERATE} ${NetCDF_LIBRARY})
    target_include_directories(sofaconvert PRIVATE ${HDF5_INCLUDE_DIRS} ${ACCELERATE_H} ${NetCDF_INCLUDE_DIR} sofaconvert common) 
endif()


if(BUILD_MODVOC_TEST AND NOT EMSCRIPTEN) 
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
    modvoc/test/main.cpp
)
set (CMAKE_CXX_STANDARD 17)
find_package(HDF5)
find_library(ACCELERATE Accelerate)

    find_library(CS_LIB_PATH ${CS_LIB})
    find_path(CSOUND_H_PATH csound.h)
    include_directories(${CSOUND_H_PATH})
  
    add_executable(modvoc_test "${SOURCES}")
    target_link_libraries(modvoc_test ${CS_LIB_PATH} ${HDF5_LIBRARIES} ${ACCELERATE})
    target_include_directories(modvoc_test PRIVATE ${HDF5_INCLUDE_DIRS} ${ACCELERATE_H} modvoc_test common) 
endif()

if(BUILD_MODVOC_OPCODE AND NOT EMSCRIPTEN)

    set(SOURCES
        modvoc/module.cpp
        modvoc/ModVoc.cpp
        modvoc/ModVoc.hpp
        common/RingBuffer.hpp
        common/RingBuffer.cpp
        common/FrameBuffer.hpp
        common/FrameBuffer.cpp
        common/SignalData/DFT.hpp
        common/SignalData/DFT.cpp
        common/SignalData/Plotter.cpp
        common/SignalData/Plotter.hpp
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
    )

    find_package(HDF5)
    find_library(ACCELERATE Accelerate)
    check_deps(BUILD_MODVOC_OPCODE HDF5_FOUND)
    if(BUILD_MODVOC_OPCODE)
        message(STATUS "HDF5_INCLUDE_DIRS: ${HDF5_INCLUDE_DIRS}")
        message(STATUS "HDF5_LIBRARIES: ${HDF5_LIBRARIES}")

        set (CMAKE_CXX_STANDARD 17)
        make_plugin(modvoc "${SOURCES}")
        target_include_directories(modvoc PRIVATE ${HDF5_INCLUDE_DIRS} ${ACCELERATE_H} modvoc common)
        target_link_libraries(modvoc ${HDF5_LIBRARIES} ${ACCELERATE})
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
        common/CsoundUtility.cpp
    )

    find_package(HDF5)
    find_library(ACCELERATE Accelerate)
    check_deps(BUILD_DYNAMICWARPING_OPCODES HDF5_FOUND)
    if(BUILD_DYNAMICWARPING_OPCODES)
        message(STATUS "HDF5_INCLUDE_DIRS: ${HDF5_INCLUDE_DIRS}")
        message(STATUS "HDF5_LIBRARIES: ${HDF5_LIBRARIES}")

        set (CMAKE_CXX_STANDARD 11)
        make_plugin(dynamicwarping "${SOURCES}")
        target_include_directories(dynamicwarping PRIVATE ${HDF5_INCLUDE_DIRS} ${ACCELERATE_H} dynamicwarping common)
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
        common/json.hpp
    )

    find_library(ACCELERATE Accelerate)
    find_library(LIBSAMPLERATE samplerate)
    find_path(LIBSAMPLERATE_INCLUDE_DIR samplerate.h)

    if(BUILD_UKFFORMANT_OPCODES)

        set (CMAKE_CXX_STANDARD 17)
        make_plugin(ukfformant "${SOURCES}")
        ADD_LIBRARY(LAPACK STATIC IMPORTED)
        ADD_LIBRARY(BLAS STATIC IMPORTED)
        ADD_LIBRARY(F2C STATIC IMPORTED)

        SET_TARGET_PROPERTIES(LAPACK PROPERTIES IMPORTED_LOCATION /usr/local/lib/liblapack.a)
        SET_TARGET_PROPERTIES(BLAS PROPERTIES IMPORTED_LOCATION /usr/local/lib/libblas.a)
        SET_TARGET_PROPERTIES(F2C PROPERTIES IMPORTED_LOCATION /usr/local/lib/libf2c.a)


        message(STATUS "LAPACK_LIBRARIES: ${LAPACK}")

        include_directories(/usr/local/include ${ACCELERATE_H})
        target_link_libraries(ukfformant F2C BLAS LAPACK ${ACCELERATE} ${HDF5_LIBRARIES} ${LIBSAMPLERATE})
    endif()
endif()

if(BUILD_JSON_OPCODES AND NOT EMSCRIPTEN)

    set(SOURCES
        json/JsonInputOpcode.hpp
        json/JsonInputOpcode.cpp
        json/cJson.c
        json/cJson.h
        json/module.cpp
        common/SignalData/Matrix.hpp
        common/SignalData/Matrix.cpp
        common/SignalData/Vector.hpp
        common/SignalData/Vector.cpp
        common/json.hpp
    )

    set (CMAKE_CXX_STANDARD 17)
    make_plugin(json "${SOURCES}")
endif()

if(BUILD_RESAMPLE_OPCODES AND NOT EMSCRIPTEN)

    find_library(LIBSAMPLERATE samplerate)
    find_path(LIBSAMPLERATE_INCLUDE_DIR samplerate.h)

    set(SOURCES
        resample/ResampleOpcodes.hpp
        resample/ResampleOpcodes.cpp
        resample/module.cpp
        common/SignalData/Matrix.hpp
        common/SignalData/Matrix.cpp
        common/SignalData/Vector.hpp
        common/SignalData/Vector.cpp
        resample/utilities.cpp
        resample/utilities.hpp
    )


    set (CMAKE_CXX_STANDARD 17)
    make_plugin(resample "${SOURCES}")
    include_directories(/usr/local/include ${LIBSAMPLERATE_INCLUDE_DIR})
    target_link_libraries(resample ${LIBSAMPLERATE} ${HDF5_LIBRARIES})

endif()



if(BUILD_TESTOPCODE AND EMSCRIPTEN)

    set(SOURCES
        testopcode/testopcode.hpp
        testopcode/testopcode.cpp
        testopcode/module.cpp
        common/json.hpp
        common/SignalData/Matrix.hpp
        common/SignalData/Matrix.cpp
        common/SignalData/MatrixMaths.hpp
        common/SignalData/MatrixMathsGeneric.cpp
        common/SignalData/Vector.hpp
        common/SignalData/Vector.cpp
        common/SignalData/VectorMaths.hpp
        common/SignalData/VectorMathsGeneric.cpp
        common/SignalData/StridedVectorMaths.hpp
        common/SignalData/StridedVectorMathsGeneric.cpp
    )

    set (CMAKE_CXX_STANDARD 17)
    make_plugin(testopcode "${SOURCES}")

    message(STATUS "LAPACK_LIBRARIES: ${LAPACK}")

    include_directories(/usr/local/include PRIVATE ${HDF5_INCLUDE_DIRS})

    target_link_libraries(testopcode F2C BLAS LAPACK ${HDF5_LIBRARIES})
endif()


check_include_file(hdf5.h HDF5_H)
add_subdirectory(common/SignalData)
