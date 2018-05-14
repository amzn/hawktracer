add_library(hawktracer_viewer
    STATIC
    base_ui.cpp
    cache.cpp
    event_db.cpp
    graph.cpp
    javascript_ui.cpp
    jsonxx.cc
    ui_controller.cpp
    websocket.cpp
    xy_graph.cpp)

target_include_directories(hawktracer_viewer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_executable(viewer main.cpp ../client/tcp_client_stream.cpp)

target_link_libraries(hawktracer_viewer boost_system pthread hawktracer_parser hawktracer)
target_link_libraries(viewer hawktracer_viewer)
