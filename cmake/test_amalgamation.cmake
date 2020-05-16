find_package(PythonInterp 3)

if(${PYTHONINTERP_FOUND})
    set(AMALGAMATION_ARGS
        ${CMAKE_SOURCE_DIR}/tools/amalgamate.py
        --root-dir ${CMAKE_SOURCE_DIR}
        --out-dir ${CMAKE_CURRENT_BINARY_DIR}
        --config-path ${CMAKE_CURRENT_BINARY_DIR}/lib/include/hawktracer/ht_config.h)

    set(AMALGAMATION_ARGS_CPP ${AMALGAMATION_ARGS} --cpp)

    add_custom_command(OUTPUT hawktracer.c
        COMMAND ${PYTHON_EXECUTABLE} ARGS ${AMALGAMATION_ARGS} VERBATIM)

    add_library(amalgamated_hawktracer_c ${HAWKTRACER_LIB_TYPE}
        hawktracer.c)

    add_custom_command(OUTPUT hawktracer.cpp
        COMMAND ${PYTHON_EXECUTABLE} ARGS ${AMALGAMATION_ARGS_CPP} VERBATIM)

    add_library(amalgamated_hawktracer_cpp ${HAWKTRACER_LIB_TYPE}
        hawktracer.cpp)
endif()
