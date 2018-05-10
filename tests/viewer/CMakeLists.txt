set(hawktracer_viewer_tests
    main_tests.cpp
    test_event_db.cpp)

DEFINE_HT_TEST(NAME viewer
    SOURCES ${hawktracer_viewer_tests}
    LINK_LIBS ${GTEST_LIBRARY} hawktracer_viewer
    RUN_PARAMS --gtest_output=xml:hawktracer_viewer_tests.xml)
