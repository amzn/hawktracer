set(HAWKTRACER_GTEST_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/test_call_graph.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_file_loader.cpp

    ${HAWKTRACER_GTEST_TEST_SOURCES}
    PARENT_SCOPE)

set(HAWKTRACER_GTEST_TEST_LIBS
    hawktracer_client
    ${HAWKTRACER_GTEST_TEST_LIBS}
    PARENT_SCOPE)
    
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test_multiple_calls.txt
          ${CMAKE_CURRENT_SOURCE_DIR}/test_3_lvls_stack_simple_calls.txt
     DESTINATION ${HT_TEST_DIR})
