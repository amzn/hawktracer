include_directories(../)
INCLUDE_DIRECTORIES(../lib/include)
add_executable(viewer main.cpp websocket.cpp ui_controller.cpp graph.cpp xy_graph.cpp base_ui.cpp jsonxx.cc event_db.cpp javascript_ui.cpp ../client/tcp_client_stream.cpp)
target_link_libraries(viewer boost_system pthread hawktracer_parser hawktracer)
