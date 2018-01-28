find_program(MSBUILD NAMES msbuild xbuild)

if(NOT MSBUILD)
    message(FATAL_ERROR "Unable to find msbuild. Install either msbuild or xbuild")
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Release" OR "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    set(CLIENT_CONFIGURATION "Release")
else()
    set(CLIENT_CONFIGURATION "Debug")
endif()

set(CLIENT_COMPILATION_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/bin/")

add_custom_target(hawktracer-client ALL
    COMMAND ${MSBUILD}
        /p:OutputPath="${CLIENT_COMPILATION_OUT_DIR}"
        /p:Configuration=${CLIENT_CONFIGURATION}
        /p:BaseIntermediateOutputPath="${CMAKE_CURRENT_BINARY_DIR}/obj/"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

install(PROGRAMS "${CLIENT_COMPILATION_OUT_DIR}/hawktracer-client.exe"
    DESTINATION bin)
