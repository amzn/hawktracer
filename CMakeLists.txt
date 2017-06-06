cmake_minimum_required(VERSION 3.0)

project(hawktracer
    VERSION 0.1.0)

set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wno-unused-parameter")

option(ENABLE_UNITTESTS "Build unit tests" ON)
option(WITH_GCOV "Build with code coverage calculation" OFF)
option(DISABLE_CLIENT "Disable hawktracer profiling client" OFF)

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
