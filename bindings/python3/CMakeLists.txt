find_package(PythonLibs 3 REQUIRED)
find_package(PythonInterp 3 REQUIRED)

configure_file(setup.py.in setup.py)

set(BUILD_LIB_PARAMS
    --lib-dirs $<TARGET_FILE_DIR:hawktracer_parser>
    --lib-dirs $<TARGET_FILE_DIR:hawktracer>
    --lib-dirs $<TARGET_FILE_DIR:hawktracer_client_utils>
    --libraries hawktracer
    --libraries hawktracer_parser
    --libraries hawktracer_client_utils
    --sources ${CMAKE_CURRENT_SOURCE_DIR}/hawktracer_python.cpp
    --sources ${CMAKE_CURRENT_SOURCE_DIR}/client_context.cpp
    --include-dirs "\"$<TARGET_PROPERTY:hawktracer_client_utils,INCLUDE_DIRECTORIES>\"")

add_custom_target(python_bindings ALL
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/setup.py build --build-base=${CMAKE_BINARY_DIR}/build ${BUILD_LIB_PARAMS})

add_dependencies(python_bindings hawktracer hawktracer_parser hawktracer_client_utils)

install(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/setup.py install --prefix=${CMAKE_INSTALL_PREFIX})")
install(FILES example-client.py DESTINATION examples/python3)
