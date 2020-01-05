set(LIB_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/listeners/test_file_dump_listener.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/listeners/test_tcp_listener.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_alloc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_allocator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_bag.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_command_line_parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_duration_conversion.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_feature_cached_string.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_feature_callstack.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_hash_map.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_common.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_event.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_listener_buffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_event_id_provider.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_global_timeline.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_registry.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_stack.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_task_scheduler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_test_events.c
    ${CMAKE_CURRENT_SOURCE_DIR}/test_thread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_timeline.cpp)

macro(SETUP_FEATURE_TEST FEATURE_NAME TEST_FILE_NAME)
    if (HT_PLATFORM_FEATURE_${FEATURE_NAME}_ENABLED)
        list(APPEND LIB_TEST_SOURCES
          ${CMAKE_CURRENT_SOURCE_DIR}/platform_features/${TEST_FILE_NAME})
    endif()
endmacro(SETUP_FEATURE_TEST)

SETUP_FEATURE_TEST(CPU_USAGE "test_cpu_usage.cpp")
SETUP_FEATURE_TEST(MEMORY_USAGE "test_memory_usage.cpp")
SETUP_FEATURE_TEST(ALLOC_HOOKS "test_alloc_hooks.cpp")

set(HAWKTRACER_GTEST_TEST_SOURCES ${HAWKTRACER_GTEST_TEST_SOURCES} ${LIB_TEST_SOURCES} PARENT_SCOPE)

add_executable(hawktracer_destroy_timeline_after_uninit_tests main_destroy_timeline_after_uninit.c)
target_link_libraries(hawktracer_destroy_timeline_after_uninit_tests hawktracer)

add_test(destroy_timeline_after_uninit_tests hawktracer_destroy_timeline_after_uninit_tests)
