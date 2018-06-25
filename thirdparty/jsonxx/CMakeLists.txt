add_library(jsonxx
    STATIC
    jsonxx.cc)

target_include_directories(jsonxx PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
