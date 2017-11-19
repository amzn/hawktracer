macro(IS_COMPILER_OK_FOR_COVERAGE COMPILER_ID COMPILER_OK)
    if ("${COMPILER_ID}" STREQUAL "Clang" OR "${COMPILER_ID}" STREQUAL "GNU")
        set(${COMPILER_OK} ON)
    endif()
endmacro(IS_COMPILER_OK_FOR_COVERAGE)

IS_COMPILER_OK_FOR_COVERAGE(${CMAKE_C_COMPILER_ID} COV_C_OK)
IS_COMPILER_OK_FOR_COVERAGE(${CMAKE_CXX_COMPILER_ID} COV_CXX_OK)

if(NOT COV_C_OK OR NOT COV_CXX_OK)
    message(FATAL_ERROR "Can't calculate coverage due to compiler incompatibility")
endif()

set(COVERAGE_COMPILER_FLAGS "-ftest-coverage -fprofile-arcs")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COVERAGE_COMPILER_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COVERAGE_COMPILER_FLAGS}")

find_program(LCOV_PATH lcov)
find_program(GENHTML_PATH genhtml)

if(NOT LCOV_PATH OR NOT GENHTML_PATH)
    message(WARNING "Unable to locate lcov or genhtml programs. Code coverage might not work.")
endif()

find_program(XDG_OPEN_PATH xdg-open)

if(NOT XDG_OPEN_PATH)
    set(XDG_OPEN_PATH cmake -E echo Coverage report generated. Open )
endif()

add_custom_target(coverage
    COMMAND ${LCOV_PATH} --directory . --capture --output-file coverage.info
    COMMAND ${LCOV_PATH} -r coverage.info /usr/\\*include/\\* \\*tests/\\* -o coverage.info
    COMMAND ${GENHTML_PATH} coverage.info --output-directory coverage_report
    COMMAND ${XDG_OPEN_PATH} coverage_report/index.html)
