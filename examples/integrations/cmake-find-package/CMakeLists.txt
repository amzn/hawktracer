project(CMakeExternalProject)
cmake_minimum_required(VERSION 3.11)

find_package(HawkTracer 0.5.0 REQUIRED)

add_executable(cmake-find-package main.cpp)
target_link_libraries(cmake-find-package HawkTracer::hawktracer)
