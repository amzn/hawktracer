set(GTEST_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/googletest")

include(ExternalProject)
ExternalProject_Add(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG master
  UPDATE_COMMAND ""
  INSTALL_COMMAND ""
  CMAKE_ARGS
    "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
    "-DBUILD_SHARED_LIBS=ON"
  BINARY_DIR "${GTEST_BIN_DIR}"
  BUILD_COMMAND "${CMAKE_COMMAND}" --build "${GTEST_BIN_DIR}" --config "Release"
  LOG_DOWNLOAD ON
  LOG_CONFIGURE ON
  LOG_BUILD ON)

ExternalProject_Get_Property(googletest source_dir)
set(GTEST_INCLUDE_DIRS ${source_dir}/googletest/include)

if(MSVC)
    set(GTEST_LIBRARY_PATH "${GTEST_BIN_DIR}/googlemock/gtest/Release/${CMAKE_STATIC_LIBRARY_PREFIX}gtest${CMAKE_STATIC_LIBRARY_SUFFIX}")
else()
    set(GTEST_LIBRARY_PATH "${GTEST_BIN_DIR}/googlemock/gtest/${CMAKE_SHARED_LIBRARY_PREFIX}gtest${CMAKE_SHARED_LIBRARY_SUFFIX}")
endif()
set(GTEST_LIBRARY hawktracer_gtest)

add_library(${GTEST_LIBRARY} UNKNOWN IMPORTED)
set_target_properties(${GTEST_LIBRARY} PROPERTIES
  IMPORTED_LOCATION ${GTEST_LIBRARY_PATH})
add_dependencies(${GTEST_LIBRARY} googletest)

include_directories(../lib/include "${GTEST_INCLUDE_DIRS}")

function(DEFINE_HT_TEST)
    set(MULTI_PARAM SOURCES LINK_LIBS RUN_PARAMS)
    cmake_parse_arguments(ht "" "NAME" "${MULTI_PARAM}" ${ARGN})
    message("${ht_RUN_PARAMS}")
    set(EXE_NAME hawktracer_test_${ht_NAME})
    add_executable(${EXE_NAME} ${ht_SOURCES})
    target_link_libraries(${EXE_NAME} ${ht_LINK_LIBS} hawktracer ${CMAKE_THREAD_LIBS_INIT})
    target_compile_definitions(${EXE_NAME} PRIVATE -DHT_COMPILE_SHARED_EXPORT)
    if ("${ht_RUN_PARAMS}" STREQUAL "")
        add_test(${ht_NAME} ${EXE_NAME})
    else()
        add_test(${ht_NAME} ${EXE_NAME} COMMAND hawktracer_tests ${ht_RUN_PARAMS})
    endif()
endfunction(DEFINE_HT_TEST)

set(hawktracer_gtest_tests_SOURCE
    main_gtest_tests.cpp
    listeners/test_file_dump_listener.cpp
    test_alloc.cpp
    test_bag.cpp
    test_feature_cached_string.cpp
    test_feature_callstack.cpp
    test_common.cpp
    test_event.cpp
    test_listener_buffer.cpp
    test_event_id_provider.cpp
    test_global_timeline.cpp
    test_registry.cpp
    test_stack.cpp
    test_test_events.c
    test_thread.cpp
    test_timeline.cpp)

DEFINE_HT_TEST(NAME gtest SOURCES ${hawktracer_gtest_tests_SOURCE} LINK_LIBS ${GTEST_LIBRARY} RUN_PARAMS --gtest_output=xml:hawktracer_tests.xml)
DEFINE_HT_TEST(NAME destroy_timeline_after_uninit SOURCES main_destroy_timeline_after_uninit.c)
