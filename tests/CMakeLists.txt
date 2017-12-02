find_package(Threads REQUIRED)

include(ExternalProject)

ExternalProject_Add(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  UPDATE_COMMAND ""
  INSTALL_COMMAND ""
  LOG_DOWNLOAD ON
  LOG_CONFIGURE ON
  LOG_BUILD ON)

ExternalProject_Get_Property(googletest source_dir)
set(GTEST_INCLUDE_DIRS ${source_dir}/googletest/include)

ExternalProject_Get_Property(googletest binary_dir)
set(GTEST_LIBRARY_PATH ${binary_dir}/googlemock/gtest/${CMAKE_FIND_LIBRARY_PREFIXES}gtest.a)
set(GTEST_LIBRARY hawktracer_gtest)

add_library(${GTEST_LIBRARY} UNKNOWN IMPORTED)
set_target_properties(${GTEST_LIBRARY} PROPERTIES
  IMPORTED_LOCATION ${GTEST_LIBRARY_PATH}
  IMPORTED_LINK_INTERFACE_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})
add_dependencies(${GTEST_LIBRARY} googletest)

include_directories(../lib/include "${GTEST_INCLUDE_DIRS}")

add_executable(hawktracer_tests
    main.cpp
    test_alloc.cpp
    test_bag.cpp
    test_callstack_base_timeline.cpp
    test_callstack_timeline.cpp
    test_common.cpp
    test_event.cpp
    test_event_id_provider.cpp
    test_global_callstack_timeline.cpp
    test_stack.cpp
    test_thread.cpp
    test_timeline.cpp
    test_timeline_registry.cpp)

target_link_libraries(hawktracer_tests ${GTEST_LIBRARY} hawktracer ${CMAKE_THREAD_LIBS_INIT})
