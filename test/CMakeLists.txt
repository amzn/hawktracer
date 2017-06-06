find_package(Threads REQUIRED)
include(ExternalProject)

ExternalProject_Add(
    gtest
    URL https://github.com/google/googletest/archive/release-1.8.0.zip
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(gtest source_dir binary_dir)
add_library(libgtest_hawktracer IMPORTED STATIC GLOBAL)
add_dependencies(libgtest_hawktracer gtest)

set_target_properties(libgtest_hawktracer PROPERTIES
    "IMPORTED_LOCATION" "${binary_dir}/googlemock/gtest/libgtest.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)

add_library(libgmock_main_hawktracer IMPORTED STATIC GLOBAL)
add_dependencies(libgmock_main_hawktracer libgtest_hawktracer)

set_target_properties(libgmock_main_hawktracer PROPERTIES
    "IMPORTED_LOCATION" "${binary_dir}/googlemock/libgmock_main.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)

include_directories(
    "${source_dir}/googletest/include"
    "${source_dir}/googlemock/include"
    "${PROJECT_SOURCE_DIR}/src")

file(GLOB TEST_SOURCES *.cpp)

add_executable(hawktracer-test ${TEST_SOURCES})

target_link_libraries(hawktracer-test hawktracer libgtest_hawktracer libgmock_main_hawktracer)

add_test(
    NAME hawktracer
    COMMAND hawktracer-test)

