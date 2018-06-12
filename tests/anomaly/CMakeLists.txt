set(hawktracer_anomaly_tests
    main_tests.cpp
    ../client/test_path.cpp
    test_pattern_matching.cpp)

file(COPY test1_config.json
          test1_pattern.txt
          test1_source.txt
          test2_config.json
          test2_pattern.txt
          test2_source.txt
          test3_config.json
          test3_pattern.txt
          test3_source.txt
          test4_config.json
          test4_pattern.txt
          test4_source.txt
          test5_config.json
          test5_pattern.txt
          test5_source.txt
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

DEFINE_HT_TEST(NAME anomaly
    SOURCES ${hawktracer_anomaly_tests}
    LINK_LIBS ${GTEST_LIBRARY} hawktracer_anomaly
    RUN_PARAMS --gtest_output=xml:hawktracer_anomaly_tests.xml)
