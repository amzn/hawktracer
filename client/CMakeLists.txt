add_subdirectory(parser)

include_directories(${CMAKE_SOURCE_DIR}/lib/include .)
add_executable(hawktracer-viewer
    main.cpp
    chrome_tracing_listener.cpp
    tcp_client_stream.cpp
    tracepoint_map.cpp)
target_link_libraries(hawktracer-viewer hawktracer_parser pthread)
