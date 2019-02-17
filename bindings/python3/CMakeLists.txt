find_package(PythonLibs 3 REQUIRED)
find_package(PythonInterp 3 REQUIRED)

function(create_python_bindings_target)
    set(options)
    set(oneValueArgs PY_MODULE_NAME)
    set(multiValueArgs BUILD_LIB_PARAMS DEPENDENCIES)
    cmake_parse_arguments(create_python_bindings_target "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(PY_MODULE_NAME "hawktracer.${create_python_bindings_target_PY_MODULE_NAME}")
    configure_file(setup.py.in setup_${PY_MODULE_NAME}.py)

    add_custom_target(python_bindings_${PY_MODULE_NAME} ALL
        COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/setup_${PY_MODULE_NAME}.py
            build --build-base=${CMAKE_BINARY_DIR}/build
            ${create_python_bindings_target_BUILD_LIB_PARAMS})

    add_dependencies(python_bindings_${PY_MODULE_NAME} ${create_python_bindings_target_DEPENDENCIES})

    install(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/setup_${PY_MODULE_NAME}.py install --prefix=${CMAKE_INSTALL_PREFIX})")
endfunction(create_python_bindings_target)

if(ENABLE_CODE_COVERAGE)
    set(EXTRA_PARAMS --libraries gcov)
endif(ENABLE_CODE_COVERAGE)

if(ENABLE_CLIENT)
    set(BUILD_LIB_PARAMS
        --lib-dirs $<TARGET_FILE_DIR:hawktracer_parser>
        --lib-dirs $<TARGET_FILE_DIR:hawktracer>
        --lib-dirs $<TARGET_FILE_DIR:hawktracer_client_utils>
        --libraries hawktracer
        --libraries hawktracer_parser
        --libraries hawktracer_client_utils
        ${EXTRA_PARAMS}
        --sources ${CMAKE_CURRENT_SOURCE_DIR}/hawktracer_client_python.cpp
        --sources ${CMAKE_CURRENT_SOURCE_DIR}/client_context.cpp
        --include-dirs "\"$<TARGET_PROPERTY:hawktracer_client_utils,INCLUDE_DIRECTORIES>\"")
    create_python_bindings_target(
        PY_MODULE_NAME client
        BUILD_LIB_PARAMS ${BUILD_LIB_PARAMS}
        DEPENDENCIES hawktracer hawktracer_parser hawktracer_client_utils)
    install(FILES plot-events.py DESTINATION examples/python3)
endif(ENABLE_CLIENT)

set(BUILD_LIB_PARAMS
    --lib-dirs $<TARGET_FILE_DIR:hawktracer>
    --libraries hawktracer
    ${EXTRA_PARAMS}
    --sources ${CMAKE_CURRENT_SOURCE_DIR}/hawktracer_core_python.cpp
    --include-dirs "\"$<TARGET_PROPERTY:hawktracer,INCLUDE_DIRECTORIES>\"")
create_python_bindings_target(
        PY_MODULE_NAME core
        BUILD_LIB_PARAMS ${BUILD_LIB_PARAMS}
        DEPENDENCIES hawktracer)
install(FILES instrumentation-base-example.py instrumentation-advanced-example.py DESTINATION examples/python3) # TODO: move to create_python_bindings_target()
