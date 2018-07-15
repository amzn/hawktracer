set(HAWKTRACER_GTEST_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/test_endianness_convert.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_event_klass.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_event.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_file_stream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test_klass_register.cpp

    ${HAWKTRACER_GTEST_TEST_SOURCES}
    PARENT_SCOPE)

set(HAWKTRACER_GTEST_TEST_LIBS
    hawktracer_parser
    ${HAWKTRACER_GTEST_TEST_LIBS}
    PARENT_SCOPE)
