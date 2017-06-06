add_executable(hawktracer-profiler
    profiler.cpp)

target_include_directories(hawktracer-profiler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../)
target_link_libraries(hawktracer-profiler hawktracer)
