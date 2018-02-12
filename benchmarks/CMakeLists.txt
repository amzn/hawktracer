if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    message(WARNING "Benchmarks should not be built in Debug mode.")
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include(ExternalProject)
ExternalProject_Add(
    googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    UPDATE_COMMAND ""
    INSTALL_COMMAND ""
    GIT_TAG v1.3.0
    CMAKE_ARGS "-DCMAKE_BUILD_TYPE=Release"
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON)

ExternalProject_Get_Property(googlebenchmark source_dir)
set(BENCHMARK_INCLUDE_DIRS ${source_dir}/include)

ExternalProject_Get_Property(googlebenchmark binary_dir)
set(BENCHMARK_LIBRARY_PATH ${binary_dir}/src/${CMAKE_STATIC_LIBRARY_PREFIX}benchmark${CMAKE_STATIC_LIBRARY_SUFFIX})
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
