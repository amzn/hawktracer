set(hawktracer_anomaly_tests
    main_tests.cpp
    ../client/test_path.cpp
    test_pattern_matching.cpp)
    
file(GLOB TEST_FILES test[1-5]_config.json test[1-5]_pattern.json test[1-5]_source.json)
file(COPY ${TEST_FILES}
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

DEFINE_HT_TEST(NAME anomaly
    SOURCES ${hawktracer_anomaly_tests}
    LINK_LIBS ${GTEST_LIBRARY} hawktracer_anomaly
    RUN_PARAMS --gtest_output=xml:hawktracer_anomaly_tests.xml)
