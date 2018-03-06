if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    message(WARNING "Benchmarks should not be built in Debug mode.")
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(GOOGLEBENCHMARK_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/googlebenchmark")

include(ExternalProject)
ExternalProject_Add(
    googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.3.0
    UPDATE_COMMAND ""
    INSTALL_COMMAND ""
    CMAKE_ARGS
        "-DCMAKE_BUILD_TYPE=Release"
        "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
    BINARY_DIR "${GOOGLEBENCHMARK_BIN_DIR}"
    BUILD_COMMAND "${CMAKE_COMMAND}" --build "${GOOGLEBENCHMARK_BIN_DIR}" --config Release
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON)

ExternalProject_Get_Property(googlebenchmark source_dir)
set(BENCHMARK_INCLUDE_DIRS ${source_dir}/include)

set(BENCHMARK_LIBRARY_PATH ${GOOGLEBENCHMARK_BIN_DIR}/src/${CMAKE_STATIC_LIBRARY_PREFIX}benchmark${CMAKE_STATIC_LIBRARY_SUFFIX})
set(BENCHMARK_LIBRARY hawktracer_googlebenchmark)

add_library(${BENCHMARK_LIBRARY} UNKNOWN IMPORTED)
set_target_properties(${BENCHMARK_LIBRARY} PROPERTIES
    IMPORTED_LOCATION ${BENCHMARK_LIBRARY_PATH})
add_dependencies(${BENCHMARK_LIBRARY} googlebenchmark)

include_directories(../lib/include "${BENCHMARK_INCLUDE_DIRS}")

add_executable(hawktracer_benchmarks
    benchmark_main.cpp
    benchmark_timeline.cpp)

target_link_libraries(hawktracer_benchmarks ${BENCHMARK_LIBRARY} hawktracer ${CMAKE_THREAD_LIBS_INIT})
