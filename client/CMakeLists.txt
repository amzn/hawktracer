include_directories(${CMAKE_SOURCE_DIR}/lib/include ..)
add_executable(hawktracer-to-json
    main.cpp
    chrome_tracing_listener.cpp
    tcp_client_stream.cpp
    tracepoint_map.cpp)
target_link_libraries(hawktracer-to-json hawktracer_parser ${CMAKE_THREAD_LIBS_INIT})

install(TARGETS hawktracer-to-json
    RUNTIME DESTINATION bin)
