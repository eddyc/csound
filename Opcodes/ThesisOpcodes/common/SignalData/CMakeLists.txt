option(BUILD_TEST_SIGNALDATA "Build the signal data tests" ON)

if(BUILD_TEST_SIGNALDATA AND NOT EMSCRIPTEN)

    set(SOURCES
        Matrix.hpp
        Matrix.cpp
        MatrixTests.cpp
        MatrixMaths.hpp
        MatrixMathsAccelerate.cpp
        Vector.hpp
        Vector.cpp
        VectorTests.cpp
        VectorMaths.hpp
        VectorMathsAccelerate.cpp
        StridedVectorMaths.hpp
        StridedVectorMathsAccelerate.cpp
        catch.hpp
        VectorMathsTests.cpp
        MatrixMathsTests.cpp
        tests.cpp
    )

    find_library(ACCELERATE Accelerate)


    message(STATUS "LAPACK_LIBRARIES: ${LAPACK}")

    include_directories(/usr/local/include  ${ACCELERATE_H})
    add_executable (SignalDataTests ${SOURCES})
    target_link_libraries(SignalDataTests F2C BLAS LAPACK  ${ACCELERATE})


endif()
