function(create_doc_coverage_target TARGET_NAME COMMAND)
    unset(COMMAND_PATH CACHE)
    find_program(COMMAND_PATH ${COMMAND})
    if ("${COMMAND_PATH}" STREQUAL "")
        message(WARNING "Can't generate target ${TARGET_NAME} because ${COMMAND} can't be found.")
    else()
        add_custom_target(${TARGET_NAME}
            COMMAND ${COMMAND_PATH} ${ARGN})
        add_dependencies(${TARGET_NAME} doc_doxygen_coverage)
    endif()
endfunction(create_doc_coverage_target)

find_package(Doxygen)
if (DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    add_custom_target(doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)

    if(WIN32 AND NOT CYGWIN)
        set(INSTALL_DOC_DIR doc)
    else()
        set(INSTALL_DOC_DIR share/doc/hawktracer)
    endif()

    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doxygen_doc/html/ DESTINATION ${INSTALL_DOC_DIR})
      
    find_package(PythonInterp 3)
    if (${PYTHONINTERP_FOUND})
        add_custom_target(doc_doxygen_coverage
            COMMAND ${PYTHON_EXECUTABLE}
                -m coverxygen
                --xml-dir ${CMAKE_CURRENT_BINARY_DIR}/doxygen_doc/xml/
                --output doc-coverage.info
                --src-dir ${CMAKE_SOURCE_DIR}/lib)
        add_dependencies(doc_doxygen_coverage doc_doxygen)

        create_doc_coverage_target(doc_doxygen_coverage_summary
            lcov
            --summary doc-coverage.info)
        create_doc_coverage_target(doc_doxygen_coverage_full
            genhtml
            --no-function-coverage --no-branch-coverage doc-coverage.info -o doc_coverage_html)
    endif()
else (DOXYGEN_FOUND)
    message(WARNING "Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)

