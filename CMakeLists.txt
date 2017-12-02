cmake_minimum_required(VERSION 3.0)

project(HawkTracer
    VERSION 0.2.0)

option(ENABLE_ASAN "Enable address sanitizer" OFF)
option(ENABLE_TESTS "Enable unit tests" OFF)
option(ENABLE_CODE_COVERAGE "Enable code coverage" OFF)

option(ENABLE_MAINTAINER_MODE "Enables maintainer mode. Overrides some flags
    (e.g. ENABLE_TESTS, ENABLE_CODE_COVERAGE)")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

# TODO must be moved somewhere else
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=c++11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -std=c90 -pedantic -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition")

if(ENABLE_MAINTAINER_MODE)
    set(ENABLE_TESTS ON)
    set(ENABLE_CODE_COVERAGE ON)
endif(ENABLE_MAINTAINER_MODE)

if(ENABLE_ASAN)
    include(asan)
endif(ENABLE_ASAN)

if(ENABLE_CODE_COVERAGE)
    include(coverage)
endif(ENABLE_CODE_COVERAGE)

add_subdirectory(lib)

if(ENABLE_TESTS)
    add_subdirectory(tests)
endif(ENABLE_TESTS)
