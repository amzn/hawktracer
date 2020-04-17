set(HT_TEST_DIR ${CMAKE_CURRENT_BINARY_DIR})

set(GTEST_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/googletest")

include(ExternalProject)
ExternalProject_Add(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.8.1
  UPDATE_COMMAND ""
  INSTALL_COMMAND ""
  CMAKE_ARGS
    "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
    "-Dgtest_force_shared_crt=ON"
  LOG_DOWNLOAD ON
  LOG_CONFIGURE ON
  LOG_BUILD ON)

ExternalProject_Get_Property(googletest source_dir)
set(GTEST_INCLUDE_DIRS ${source_dir}/googletest/include)

ExternalProject_Get_Property(googletest binary_dir)
set(GTEST_LIBS_DIRS ${binary_dir}/googlemock/gtest)

if(MSVC)
    set(GTEST_LIBRARY_PATH "${GTEST_LIBS_DIRS}/$(Configuration)/${CMAKE_STATIC_LIBRARY_PREFIX}gtest${CMAKE_STATIC_LIBRARY_SUFFIX}")
else()
    set(GTEST_LIBRARY_PATH "${GTEST_LIBS_DIRS}/${CMAKE_STATIC_LIBRARY_PREFIX}gtest${CMAKE_STATIC_LIBRARY_SUFFIX}")
endif()

set(GTEST_LIBRARY hawktracer_gtest)
add_library(${GTEST_LIBRARY} UNKNOWN IMPORTED)
set_target_properties(${GTEST_LIBRARY} PROPERTIES
  IMPORTED_LOCATION ${GTEST_LIBRARY_PATH})
add_dependencies(${GTEST_LIBRARY} googletest)

set(HAWKTRACER_GTEST_TEST_LIBS
    ${GTEST_LIBRARY})
set(HAWKTRACER_GTEST_TEST_SOURCES
    main_gtest_tests.cpp
    test_path.cpp)

add_subdirectory(lib)
add_subdirectory(bindings)

if(ENABLE_CLIENT)
    add_subdirectory(parser)
    add_subdirectory(client_utils)
    add_subdirectory(client)
    add_subdirectory(integration)
endif(ENABLE_CLIENT)

find_package(Threads REQUIRED)

add_executable(hawktracer_gtest_tests ${HAWKTRACER_GTEST_TEST_SOURCES})
target_link_libraries(hawktracer_gtest_tests hawktracer Threads::Threads ${HAWKTRACER_GTEST_TEST_LIBS})
target_include_directories(hawktracer_gtest_tests PRIVATE ${GTEST_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})

CHECK_INCLUDE_FILE("valgrind/valgrind.h" VALGRIND_CHECK_ENABLED)
if (VALGRIND_CHECK_ENABLED)
    target_compile_definitions(hawktracer_gtest_tests PRIVATE VALGRIND_CHECK_ENABLED)
endif()

add_test(gtest_tests hawktracer_gtest_tests --gtest_output=xml:hawktracer_gtest_tests.xml)
