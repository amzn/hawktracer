add_library(hawktracer_parser
    SHARED
    debug_event_listener.cpp
    event.cpp
    event_klass.cpp
    file_stream.cpp
    klass_register.cpp
    protocol_reader.cpp)

target_link_libraries(hawktracer_parser hawktracer)
target_include_directories(hawktracer_parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

install(TARGETS hawktracer_parser
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
install(DIRECTORY include/hawktracer DESTINATION include)
