set(hawktracer_client_utils_tests
    main_tests.cpp
    test_command_line_parser.cpp
    test_stream_factory.cpp)

DEFINE_HT_TEST(NAME client_utils
    SOURCES ${hawktracer_client_utils_tests}
    LINK_LIBS ${GTEST_LIBRARY} client_utils
    RUN_PARAMS --gtest_output=xml:hawktracer_client_utils_tests.xml)
