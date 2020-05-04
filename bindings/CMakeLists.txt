if(ENABLE_PYTHON_BINDINGS)
    add_subdirectory(python3)
endif()

if(ENABLE_NODEJS_BINDINGS)
    add_subdirectory(nodejs)
endif()
