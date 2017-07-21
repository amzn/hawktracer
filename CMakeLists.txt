cmake_minimum_required(VERSION 3.0)

project(hawktracer
    VERSION 0.2.0)

set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wno-unused-parameter")

option(ENABLE_UNITTESTS "Build unit tests" ON)
option(WITH_GCOV "Build with code coverage calculation" OFF)
option(DISABLE_CLIENT "Disable hawktracer profiling client" OFF)

set(ACTION_LABEL_SIZE "64" CACHE STRING "Size (in bits) of the action label field")

if((NOT "${ACTION_LABEL_SIZE}" STREQUAL "32") AND (NOT "${ACTION_LABEL_SIZE}" STREQUAL "64"))
    message(FATAL_ERROR "Action label size ${ACTION_LABEL_SIZE} not supported (only following sizes: 32 and 64 are supported")
else()
    add_definitions(-DACTION_LABEL_SIZE=${ACTION_LABEL_SIZE})
endif()

if(WITH_GCOV)
    add_definitions(-fprofile-arcs -ftest-coverage)
    link_libraries(gcov)
endif(WITH_GCOV)

include_directories(src)
add_subdirectory(src)

if(ENABLE_UNITTESTS)
    enable_testing()
    add_subdirectory(test)
endif(ENABLE_UNITTESTS)
