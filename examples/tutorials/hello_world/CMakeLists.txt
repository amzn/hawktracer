add_executable(hello-world-c hawktracer-hello-world.cpp)
target_link_libraries(hello-world-c hawktracer)

add_executable(hello-world-cpp hawktracer-hello-world.c)
target_link_libraries(hello-world-cpp hawktracer)
