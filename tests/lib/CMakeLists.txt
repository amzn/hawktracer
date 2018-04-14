set(hawktracer_gtest_tests_SOURCE
    main_gtest_tests.cpp
    listeners/test_file_dump_listener.cpp
    test_alloc.cpp
    test_bag.cpp
    test_feature_cached_string.cpp
    test_feature_callstack.cpp
    test_common.cpp
    test_event.cpp
    test_listener_buffer.cpp
    test_event_id_provider.cpp
    test_global_timeline.cpp
    test_registry.cpp
    test_stack.cpp
    test_test_events.c
    test_thread.cpp
    test_timeline.cpp)

DEFINE_HT_TEST(NAME gtest SOURCES ${hawktracer_gtest_tests_SOURCE} LINK_LIBS ${GTEST_LIBRARY} RUN_PARAMS --gtest_output=xml:hawktracer_tests.xml)
DEFINE_HT_TEST(NAME destroy_timeline_after_uninit SOURCES main_destroy_timeline_after_uninit.c)
