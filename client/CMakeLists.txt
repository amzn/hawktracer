add_executable(hawktracer-to-json
    main.cpp
    callgrind_listener.cpp
    chrome_tracing_listener.cpp
    tcp_client_stream.cpp
    tracepoint_map.cpp)
target_link_libraries(hawktracer-to-json hawktracer_parser)

install(TARGETS hawktracer-to-json
    RUNTIME DESTINATION bin)
