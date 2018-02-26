include_directories(${CMAKE_SOURCE_DIR}/lib/include ..)

add_library(hawktracer_parser
    STATIC
    debug_event_listener.cpp
    event.cpp
    event_klass.cpp
    file_stream.cpp
    klass_register.cpp
    protocol_reader.cpp)
