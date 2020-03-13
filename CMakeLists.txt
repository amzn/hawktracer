cmake_minimum_required(VERSION 3.1)

project(HawkTracer
    VERSION 0.10.0)

include(GNUInstallDirs)

option(ENABLE_ASAN "Enable address sanitizer" OFF)
option(ENABLE_TESTS "Enable unit tests" OFF)
option(ENABLE_CODE_COVERAGE "Enable code coverage" OFF)
option(ENABLE_BENCHMARKS "Enable benchmarks" OFF)
option(ENABLE_EXAMPLES "Enable examples" OFF)
option(ENABLE_DOC "Build documentation" ON)
option(ENABLE_CLIENT "Enable client application for parsing HawkTracer stream" ON)
option(ENABLE_PYTHON_BINDINGS "Enable Python bindings (requires ENABLE_CLIENT=ON)" OFF)
option(ENABLE_NODEJS_BINDINGS "Enable Node.js bindings (requires ENABLE_CLIENT=ON)" OFF)

option(ENABLE_POSITION_INDEPENDENT_CODE "Enable position independent code of
    the HawkTracer library. For most of the casses, it adds -fPIC flag to a compiler." OFF)

option(BUILD_STATIC_LIB "Build static hawktracer library" OFF)

option(ENABLE_MAINTAINER_MODE "Enables maintainer mode. Overrides some flags
    (e.g. ENABLE_TESTS, ENABLE_CODE_COVERAGE, ENABLE_BENCHMARKS)")

option(ENABLE_RELEASE_MODE "Enables release mode. This flag should be used
    to compile and release product.")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# PLATFORM FEATURES
include(platform_features)
define_platform_feature(CPU_USAGE "cpu_usage.c" DEFAULT)
define_platform_feature(MEMORY_USAGE "memory_usage.c" DEFAULT)
define_platform_feature(ALLOC_HOOKS "alloc_hooks.c" OFF)

# VARIABLES
if(WIN32 AND NOT CYGWIN)
    set(INSTALL_DOC_DIR doc)
else()
    set(INSTALL_DOC_DIR ${CMAKE_INSTALL_DATAROOTDIR}/doc/hawktracer-${PROJECT_VERSION})
endif()

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_C_STANDARD 99)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

if(ENABLE_MAINTAINER_MODE)
    set(ENABLE_PYTHON_BINDINGS ON)
    set(ENABLE_TESTS ON)
    set(ENABLE_CODE_COVERAGE ON)
    set(ENABLE_BENCHMARKS ON)
    set(ENABLE_EXAMPLES ON)
endif(ENABLE_MAINTAINER_MODE)

if(ENABLE_RELEASE_MODE)
    set(ENABLE_TESTS ON)
    set(ENABLE_BENCHMARKS ON)
    set(ENABLE_EXAMPLES ON)
    set(ENABLE_PYTHON_BINDINGS ON)
endif(ENABLE_RELEASE_MODE)

include(warnings)
include(test_amalgamation)

if(ENABLE_ASAN)
    include(asan)
endif(ENABLE_ASAN)

if(ENABLE_CODE_COVERAGE)
    include(coverage)
endif(ENABLE_CODE_COVERAGE)

if(ENABLE_DOC)
    include(documentation)
endif(ENABLE_DOC)

if(BUILD_STATIC_LIB)
    set(HAWKTRACER_LIB_TYPE STATIC)
else ()
    set(HAWKTRACER_LIB_TYPE SHARED)
endif(BUILD_STATIC_LIB)

add_subdirectory(lib)

if(ENABLE_CLIENT)
    add_subdirectory(parser)
    add_subdirectory(client_utils)
    add_subdirectory(client)
endif(ENABLE_CLIENT)

add_subdirectory(bindings)
add_subdirectory(tools_integration)

if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif(ENABLE_TESTS)

if(ENABLE_EXAMPLES)
    add_subdirectory(examples)
endif(ENABLE_EXAMPLES)
install(DIRECTORY examples DESTINATION ${INSTALL_DOC_DIR})

if(ENABLE_BENCHMARKS)
    add_subdirectory(benchmarks)
endif(ENABLE_BENCHMARKS)

include(packager)
