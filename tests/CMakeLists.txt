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

function(DEFINE_HT_TEST)
    set(MULTI_PARAM SOURCES LINK_LIBS RUN_PARAMS)
    cmake_parse_arguments(ht "" "NAME" "${MULTI_PARAM}" ${ARGN})
    set(EXE_NAME hawktracer_test_${ht_NAME})
    add_executable(${EXE_NAME} ${ht_SOURCES})
    target_link_libraries(${EXE_NAME} ${ht_LINK_LIBS} hawktracer)
    target_include_directories(${EXE_NAME} PRIVATE ${GTEST_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR})
    if ("${ht_RUN_PARAMS}" STREQUAL "")
        add_test(${ht_NAME} ${EXE_NAME})
    else()
        add_test(${ht_NAME} ${EXE_NAME} COMMAND hawktracer_tests ${ht_RUN_PARAMS})
    endif()
endfunction(DEFINE_HT_TEST)

add_subdirectory(lib)

if(ENABLE_CLIENT)
    add_subdirectory(parser)
    add_subdirectory(client_utils)
    add_subdirectory(client)
endif(ENABLE_CLIENT)
