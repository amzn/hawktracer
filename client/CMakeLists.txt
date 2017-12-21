find_program(MSBUILD NAMES msbuild xbuild)

if(NOT MSBUILD)
    message(FATAL_ERROR "Unable to find msbuild. Install either msbuild or xbuild")
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Release" OR "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    set(CLIENT_CONFIGURATION "Release")
else()
    set(CLIENT_CONFIGURATION "Debug")
endif()

add_custom_target(hawktracer-client ALL
    COMMAND ${MSBUILD}
        /p:OutputPath="${CMAKE_CURRENT_BINARY_DIR}/bin/"
        /p:Configuration=${CLIENT_CONFIGURATION}
        /p:BaseIntermediateOutputPath="${CMAKE_CURRENT_BINARY_DIR}/obj/"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
