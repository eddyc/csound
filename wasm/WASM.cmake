if(EMSCRIPTEN)
  set(BUILD_PLUGINS_DIR "plugins")
  set(DUSE_COMPILER_OPTIMIZATIONS 0)
  set(WASM 1)
  set(INIT_STATIC_MODULES 0)
  set(USE_DOUBLE 0)
  set(BUILD_MULTI_CORE 0)
  set(BUILD_JACK_OPCODES 0)
  set(HAVE_BIG_ENDIAN 0)
  set(CMAKE_16BIT_TYPE "unsigned short")
  set(HAVE_STRTOD_L 0)
  set(HAVE_ATOMIC_BUILTIN 0)
  set(HAVE_SPRINTF_L NO)
  set(USE_GETTEXT)

  function(download_file url filename)

    if(NOT EXISTS ${filename})
      file(DOWNLOAD ${url} ${filename} TIMEOUT 60)
      execute_process(COMMAND ${CMAKE_COMMAND}
                              -E
                              tar
                              -xf
                              ${filename}
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    endif()
  endfunction(download_file)

  download_file(https://github.com/eddyc/csound/releases/download/0.0.1/deps.zip
                ./deps.zip)

  set(LIBSNDFILE_LIBRARY ${CMAKE_CURRENT_BINARY_DIR}/deps/lib/libsndfile.a)

  set(SNDFILE_H_PATH ${CMAKE_CURRENT_BINARY_DIR}/deps/include)
  list(APPEND libcsound_SRCS
              ${CMAKE_CURRENT_SOURCE_DIR}/Emscripten/src/CsoundObj.c
              ${CMAKE_CURRENT_SOURCE_DIR}/Emscripten/src/FileList.c)
  set(CSOUNDLIB_STATIC_EMSCRIPTEN "csound-static-emscripten")
  add_executable(${CSOUNDLIB_STATIC_EMSCRIPTEN} ${libcsound_SRCS})
  set_target_properties(${CSOUNDLIB_STATIC_EMSCRIPTEN}
                        PROPERTIES OUTPUT_NAME libcsound)
  set(
    CMAKE_C_FLAGS
    "${CMAKE_C_FLAGS} -s \"BINARYEN_METHOD='native-wasm'\"  -s ERROR_ON_UNDEFINED_SYMBOLS=0 -s ENVIRONMENT=web,worker -s MODULARIZE=1 -s SINGLE_FILE=1 -s BINARYEN_ASYNC_COMPILATION=0 -s ASSERTIONS=0 -s EXPORT_NAME=\"'libcsound'\""
    )
  target_compile_options(${CSOUNDLIB_STATIC_EMSCRIPTEN}
                         PRIVATE ${libcsound_CFLAGS})
  target_link_libraries(${CSOUNDLIB_STATIC_EMSCRIPTEN} ${libcsound_LIBS})

endif()
