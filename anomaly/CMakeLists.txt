add_library(hawktracer_anomaly
    STATIC
    config.cpp
    event_listener.cpp
    jsonxx.cc
    ordered_tree_editing_distance.cpp
    pattern_matching.cpp
    patterns.cpp
    source.cpp
    tree_file_loader.cpp)

target_include_directories(hawktracer_anomaly PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_executable(hawktracer-anomaly main.cpp)

target_link_libraries(hawktracer_anomaly hawktracer_parser)
target_link_libraries(hawktracer-anomaly hawktracer_anomaly)

install(TARGETS hawktracer-anomaly
    RUNTIME DESTINATION bin)
