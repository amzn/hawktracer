set(hawktracer_parser_tests
    main_tests.cpp
    test_event_klass.cpp
    test_event.cpp)

DEFINE_HT_TEST(NAME parser
    SOURCES ${hawktracer_parser_tests}
    LINK_LIBS ${GTEST_LIBRARY} hawktracer_parser
    RUN_PARAMS --gtest_output=xml:hawktracer_parser_tests.xml)
