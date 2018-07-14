add_library(hawktracer_client
    STATIC
    call_graph.cpp
    callgrind_converter.cpp
    chrome_trace_converter.cpp
    converter.cpp
    tracepoint_map.cpp)

target_include_directories(hawktracer_client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_executable(hawktracer-converter main.cpp)

target_link_libraries(hawktracer_client hawktracer_parser client_utils)
target_link_libraries(hawktracer-converter hawktracer_client)

install(TARGETS hawktracer-converter
    RUNTIME DESTINATION bin)
