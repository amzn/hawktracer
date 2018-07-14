add_library(client_utils
    STATIC
    command_line_parser.cpp
    stream_factory.cpp
    tcp_client_stream.cpp)

target_link_libraries(client_utils hawktracer_parser)
