macro(_platform_feature_set FEATURE_NAME FEATURE_SOURCE)
    if (EXISTS ${FEATURE_SOURCE})
        set(HT_PLATFORM_FEATURE_${FEATURE_NAME}_SOURCE ${FEATURE_SOURCE})
        set(HT_PLATFORM_FEATURE_${FEATURE_NAME}_ENABLED ON)
        message(STATUS "Feature ${FEATURE_NAME} enabled. Used file: ${FEATURE_SOURCE}")
    else()
        message(WARNING "Feature ${FEATURE_NAME} will be disabled! File ${FEATURE_SOURCE} could not be found!")
    endif()
endmacro(_platform_feature_set)

macro(define_platform_feature FEATURE_NAME FEATURE_DEFAULT_FILE)
    set(FEATURE_${FEATURE_NAME}_PLATFORM "" CACHE STRING "Platform for ${FEATURE_NAME} feature.
        Should not be used with FEATURE_${FEATURE_NAME}_SOURCE")
    set(FEATURE_${FEATURE_NAME}_SOURCE "" CACHE STRING "Absolute source file path for ${FEATURE_NAME} feature.
        Should not be used with FEATURE_${FEATURE_NAME}_PLATFORM")

    set(FEATURE_PLATFORM "${FEATURE_${FEATURE_NAME}_PLATFORM}")
    set(FEATURE_SOURCE "${FEATURE_${FEATURE_NAME}_SOURCE}")
    if (NOT "${FEATURE_SOURCE}" STREQUAL "")
        _platform_feature_set(${FEATURE_NAME} ${FEATURE_SOURCE})
    endif()

    string(TOLOWER "${CMAKE_SYSTEM_NAME}" SYSTEM_NAME)
    set(FEATURE_SOURCE "${CMAKE_SOURCE_DIR}/lib/platform/${SYSTEM_NAME}/${FEATURE_DEFAULT_FILE}")
    if ("${FEATURE_PLATFORM}" STREQUAL "" AND EXISTS ${FEATURE_SOURCE})
        set(FEATURE_PLATFORM ${SYSTEM_NAME})
    endif()

    if (NOT HT_PLATFORM_FEATURE_${FEATURE_NAME}_ENABLED AND NOT ${FEATURE_PLATFORM} STREQUAL "")
        set(FEATURE_SOURCE "${CMAKE_SOURCE_DIR}/lib/platform/${FEATURE_PLATFORM}/${FEATURE_DEFAULT_FILE}")
        _platform_feature_set(${FEATURE_NAME} ${FEATURE_SOURCE})
    endif()
endmacro(define_platform_feature)
