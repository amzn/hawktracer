############ pkg-config ############
set(INSTALL_PKGCONFIG_DIR "share/pkgconfig")

set(INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include")
set(INSTALL_LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib")

configure_file(hawktracer.pc.in hawktracer.pc @ONLY)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/hawktracer.pc"
    DESTINATION ${INSTALL_PKGCONFIG_DIR})


############ cmake ############
include(CMakePackageConfigHelpers)

function(configure_hawktracer_config_file INCLUDE_PATH FILE_NAME)
    set(INCLUDE_INSTALL_DIR ${INCLUDE_PATH})
    configure_package_config_file(HawkTracerConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/${FILE_NAME}
        INSTALL_DESTINATION ${INSTALL_CMAKE_DIR}
        PATH_VARS INCLUDE_INSTALL_DIR INSTALL_CMAKE_DIR
        NO_CHECK_REQUIRED_COMPONENTS_MACRO)
endfunction(configure_hawktracer_config_file)

if(WIN32 AND NOT CYGWIN)
    set(INSTALL_CMAKE_DIR CMake)
else()
    set(INSTALL_CMAKE_DIR lib/cmake/HawkTracer)
endif()

export(TARGETS hawktracer
    FILE "${CMAKE_CURRENT_BINARY_DIR}/HawkTracerTargets.cmake"
    NAMESPACE HawkTracer::)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/HawkTracerConfigVersion.cmake
    COMPATIBILITY SameMajorVersion)

# for install-tree
configure_hawktracer_config_file(include HawkTracerConfigToInstall.cmake)
#for build-tree
configure_hawktracer_config_file("${PROJECT_SOURCE_DIR}/lib/include" HawkTracerConfig.cmake)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/HawkTracerConfigVersion.cmake
    DESTINATION "${INSTALL_CMAKE_DIR}")
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/HawkTracerConfigToInstall.cmake
    DESTINATION "${INSTALL_CMAKE_DIR}"
    RENAME HawkTracerConfig.cmake)
install(EXPORT HawkTracerTargets
    NAMESPACE HawkTracer::
    DESTINATION "${INSTALL_CMAKE_DIR}")
