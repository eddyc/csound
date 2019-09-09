option(BUILD_FASTCONVOLVE_OPCODES "Build the fastconvolve opcodes" ON)
set(CMAKE_CXX_STANDARD 17)

if(BUILD_FASTCONVOLVE_OPCODES AND NOT EMSCRIPTEN)

  set(SOURCES
      common/Vector.hpp
      common/Vector.cpp
      common/FrameBuffer.hpp
      common/FrameBuffer.cpp
      module.cpp
      FastConvolve.cpp
      FastConvolve.hpp)

  find_library(ACCELERATE Accelerate)

  make_plugin(fastconvolve "${SOURCES}")
  include_directories(${ACCELERATE_H})
  target_link_libraries(fastconvolve ${ACCELERATE})
endif()
