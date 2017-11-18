cmake_minimum_required(VERSION 3.0)

project(HawkTracer
    VERSION 0.2.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wno-unused-parameter")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic -Wno-unused-parameter")


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftest-coverage -fprofile-arcs")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ftest-coverage -fprofile-arcs")


#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lasan -fsanitize=address -fno-omit-frame-pointer")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lasan -fsanitize=address -fno-omit-frame-pointer")

add_subdirectory(lib)
add_subdirectory(tests)
