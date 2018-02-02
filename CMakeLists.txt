cmake_minimum_required(VERSION 3.0)

project(HawkTracer
    VERSION 0.2.0)

option(ENABLE_ASAN "Enable address sanitizer" OFF)
option(ENABLE_TESTS "Enable unit tests" OFF)
option(ENABLE_CODE_COVERAGE "Enable code coverage" OFF)
option(ENABLE_BENCHMARKS "Enable benchmarks" OFF)
option(ENABLE_EXAMPLES "Enable examples" OFF)
option(ENABLE_STATIC_LIB "Build static hawktracer library" OFF)
option(ENABLE_SHARED_LIB "Build shared hawktracer library" ON)

option(ENABLE_MAINTAINER_MODE "Enables maintainer mode. Overrides some flags
    (e.g. ENABLE_TESTS, ENABLE_CODE_COVERAGE, ENABLE_BENCHMARKS)")

option(ENABLE_RELEASE_MODE "Enables release mode. This flag should be used
    to compile and release product.")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

# TODO must be moved somewhere else
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=c++11 -Wno-unused-function")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -std=c99 -pedantic -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition -Wno-unused-function")

if(ENABLE_MAINTAINER_MODE)
    set(ENABLE_TESTS ON)
    set(ENABLE_CODE_COVERAGE ON)
    set(ENABLE_BENCHMARKS ON)
    set(ENABLE_EXAMPLES ON)
endif(ENABLE_MAINTAINER_MODE)

if(ENABLE_RELEASE_MODE)
    set(ENABLE_STATIC_LIB ON)
    set(ENABLE_SHARED_LIB ON)
    set(ENABLE_TESTS ON)
    set(ENABLE_BENCHMARKS ON)
    set(ENABLE_EXAMPLES ON)
endif(ENABLE_RELEASE_MODE)

if(ENABLE_ASAN)
    include(asan)
endif(ENABLE_ASAN)

if(ENABLE_CODE_COVERAGE)
    include(coverage)
endif(ENABLE_CODE_COVERAGE)

add_subdirectory(lib)
add_subdirectory(client)

if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif(ENABLE_TESTS)

if(ENABLE_EXAMPLES)
    add_subdirectory(examples)
endif(ENABLE_EXAMPLES)

if(ENABLE_BENCHMARKS)
    add_subdirectory(benchmarks)
endif(ENABLE_BENCHMARKS)

include(cmake/packager.cmake)
