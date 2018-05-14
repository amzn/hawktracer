macro(add_example name)
    add_executable(example_${name} example_${name}.cpp)
    target_include_directories(example_${name}  PRIVATE "${PROJECT_BINARY_DIR}/lib/include")
    target_link_libraries(example_${name} hawktracer ${CMAKE_THREAD_LIBS_INIT})
    install(FILES example_${name}.cpp DESTINATION examples)
endmacro(add_example)

add_example(simple_callstack)
