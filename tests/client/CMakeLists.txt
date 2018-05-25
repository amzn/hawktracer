set(hawktracer_client_tests
    main_tests.cpp
    test_call_graph.cpp
    test_path.cpp)
    
file(COPY test_multiple_calls.txt
          test_3_lvls_stack_simple_calls.txt 
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

DEFINE_HT_TEST(NAME client
    SOURCES ${hawktracer_client_tests}
    LINK_LIBS ${GTEST_LIBRARY} hawktracer_client
    RUN_PARAMS --gtest_output=xml:hawktracer_client_tests.xml)
