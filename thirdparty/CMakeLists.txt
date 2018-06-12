add_library(thirdparty
    STATIC
    jsonxx/jsonxx.cc)

target_include_directories(thirdparty PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
