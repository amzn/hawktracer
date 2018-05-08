############ pkg-config ############
set(INSTALL_PKGCONFIG_DIR "${CMAKE_INSTALL_PREFIX}/share/pkgconfig")
set(INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(INSTALL_LIB_DIR ${CMAKE_INSTALL_PREFIX}/lib)
set(INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)

configure_file(hawktracer.pc.in hawktracer.pc @ONLY)
install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/hawktracer.pc"
    DESTINATION "${INSTALL_PKGCONFIG_DIR}")
