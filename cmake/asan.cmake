if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    message(FATAL_ERROR "ASAN cannot be built in Release configuration")
endif()

set(CLANG_ASAN_FLAGS "-fsanitize=address -fno-omit-frame-pointer")

macro(CHECK_ASAN COMPILER_ID ASAN_FLAGS)
    if ("${COMPILER_ID}" STREQUAL "Clang")
        set(${ASAN_FLAGS} ${CLANG_ASAN_FLAGS})
    elseif ("${COMPILER_ID}" STREQUAL "GNU")
        set(${ASAN_FLAGS})
        link_libraries(asan)
    else()
        message(WARNING "Asan not supported for ${CMAKE_CXX_COMPILER_ID} compiler")
    endif()
endmacro()

CHECK_ASAN(${CMAKE_C_COMPILER_ID} ASAN_C_FLAGS)
CHECK_ASAN(${CMAKE_CXX_COMPILER_ID} ASAN_CXX_FLAGS)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ASAN_C_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ASAN_CXX_FLAGS}")
