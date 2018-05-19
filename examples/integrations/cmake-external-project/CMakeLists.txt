project(CMakeExternalProject)
cmake_minimum_required(VERSION 3.11)

include(hawktracer.cmake)

add_executable(cmake-external-project main.cpp)
target_link_libraries(cmake-external-project hawktracer)
