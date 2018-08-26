add_library(hawktracer_client_utils
    ${HAWKTRACER_LIB_TYPE}
    command_line_parser.cpp
    stream_factory.cpp
    tcp_client_stream.cpp)

set_target_properties(hawktracer_client_utils PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)

target_link_libraries(hawktracer_client_utils hawktracer_parser)
target_include_directories(hawktracer_client_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

install(TARGETS hawktracer_client_utils
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

install(DIRECTORY include/hawktracer DESTINATION include)
