macro(_platform_feature_set FEATURE_NAME FEATURE_SOURCE)
    if (ENABLE_${FEATURE_NAME}_FEATURE)
        if (EXISTS ${FEATURE_SOURCE})
            set(HT_PLATFORM_FEATURE_${FEATURE_NAME}_SOURCE ${FEATURE_SOURCE})
            set(HT_PLATFORM_FEATURE_${FEATURE_NAME}_ENABLED ON)
            message(STATUS "Feature ${FEATURE_NAME} enabled. Used file: ${FEATURE_SOURCE}")
        else()
            message(WARNING "Feature ${FEATURE_NAME} will be disabled! File ${FEATURE_SOURCE} could not be found!")
        endif()
    endif()
endmacro(_platform_feature_set)

macro(_validate_feature_enabled FEATURE_NAME VARIABLE)
    if (NOT "${FEATURE_${FEATURE_NAME}_${VARIABLE}}" STREQUAL "" AND NOT ENABLE_${FEATURE_NAME}_FEATURE)
        message(WARNING "Value FEATURE_${FEATURE_NAME}_${VARIABLE} defined but option ENABLE_${FEATURE_NAME}_FEATURE is not enabled. "
            "Enable ENABLE_${FEATURE_NAME}_FEATURE option to enable ${FEATURE_NAME} feature.")
    endif()
endmacro(_validate_feature_enabled)

macro(define_platform_feature FEATURE_NAME FEATURE_DEFAULT_FILE DEFAULT_ENABLE)
    if (NOT "${DEFAULT_ENABLE}" STREQUAL "OFF" AND NOT "${ENABLE_${FEATURE_NAME}_FEATURE}" STREQUAL "OFF")
        set(OPTION_VALUE ON)
    else()
        set(OPTION_VALUE OFF)
    endif()

    set(ENABLE_${FEATURE_NAME}_FEATURE ${OPTION_VALUE} CACHE BOOL "${FEATURE_NAME} feature." FORCE)

    _validate_feature_enabled(${FEATURE_NAME} PLATFORM)
    _validate_feature_enabled(${FEATURE_NAME} SOURCE)

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
    if ("${FEATURE_PLATFORM}" STREQUAL "" AND EXISTS "${CMAKE_SOURCE_DIR}/lib/platform/${SYSTEM_NAME}/${FEATURE_DEFAULT_FILE}")
        set(FEATURE_PLATFORM ${SYSTEM_NAME})
    endif()

    if (NOT HT_PLATFORM_FEATURE_${FEATURE_NAME}_ENABLED AND NOT ${FEATURE_PLATFORM} STREQUAL "" AND "${FEATURE_SOURCE}" STREQUAL "")
        set(FEATURE_SOURCE "${CMAKE_SOURCE_DIR}/lib/platform/${FEATURE_PLATFORM}/${FEATURE_DEFAULT_FILE}")
        _platform_feature_set(${FEATURE_NAME} ${FEATURE_SOURCE})
    endif()

    if ("${DEFAULT_ENABLE}" STREQUAL "DEFAULT")
        if (NOT HT_PLATFORM_FEATURE_${FEATURE_NAME}_ENABLED)
            set(OPTION_VALUE OFF)
        else()
            set(OPTION_VALUE ON)
        endif()
        set(ENABLE_${FEATURE_NAME}_FEATURE ${OPTION_VALUE} CACHE BOOL "${FEATURE_NAME} feature." FORCE)
    endif()

    if (ENABLE_${FEATURE_NAME}_FEATURE AND NOT HT_PLATFORM_FEATURE_${FEATURE_NAME}_ENABLED)
        message(FATAL_ERROR "Feature ${FEATURE_NAME} enabled, but no implementation found. Specify either "
           "FEATURE_${FEATURE_NAME}_PLATFORM or FEATURE_${FEATURE_NAME}_SOURCE option, or clear both options to use default implementation (if exists).")
    endif()
endmacro(define_platform_feature)
