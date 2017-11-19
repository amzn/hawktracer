include_directories(../lib/include)

add_executable(hawktracer_tests
    main.cpp
    test_alloc.cpp
    test_common.cpp
    test_event_id_provider.cpp
    test_timeline.cpp
    test_callstack_base_timeline.cpp
    test_callstack_timeline.cpp)

target_link_libraries(hawktracer_tests gtest hawktracer)
