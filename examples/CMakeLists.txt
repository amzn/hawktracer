include_directories(../lib/include)

macro(add_example name)
    add_executable(example_${name} example_${name}.cpp)
    target_link_libraries(example_${name} hawktracer)
endmacro(add_example)

add_example(simple_callstack)
