add_library(hawktracer_client_utils
    SHARED
    command_line_parser.cpp
    stream_factory.cpp
    tcp_client_stream.cpp)

target_link_libraries(hawktracer_client_utils hawktracer_parser)
target_include_directories(hawktracer_client_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

install(TARGETS hawktracer_client_utils
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

install(DIRECTORY include/hawktracer DESTINATION include)
