set(HAWKTRACER_LISTENER_HEADERS
    include/hawktracer/listeners/file_dump_listener.h
    include/hawktracer/listeners/tcp_listener.h)

set(HAWKTRACER_CORE_HEADERS
    include/hawktracer/alloc.h
    include/hawktracer/base_types.h
    include/hawktracer/core_events.h
    include/hawktracer/duration_conversion.h
    include/hawktracer/event_id_provider.h
    include/hawktracer/event_macros.h
    include/hawktracer/event_macros_impl.h
    include/hawktracer/events.h
    include/hawktracer/feature.h
    include/hawktracer/feature_cached_string.h
    include/hawktracer/feature_callstack.h
    include/hawktracer/global_timeline.h
    include/hawktracer/init.h
    include/hawktracer/listener_buffer.h
    include/hawktracer/listeners.h
    include/hawktracer/macros.h
    include/hawktracer/mkcreflect.h
    include/hawktracer/monotonic_clock.h
    include/hawktracer/registry.h
    include/hawktracer/scoped_tracepoint.h
    include/hawktracer/scoped_tracepoint.hpp
    include/hawktracer/string_scoped_tracepoint.h
    include/hawktracer/system_info.h
    include/hawktracer/task_scheduler.h
    include/hawktracer/thread.h
    include/hawktracer/timeline.h
    include/hawktracer/timeline_listener.h)

set(HAWKTRACER_LIB_HEADERS ${HAWKTRACER_LISTENER_HEADERS} ${HAWKTRACER_CORE_HEADERS})

set(HAWKTRACER_LISTENERS_SOURCES
    listeners/file_dump_listener.c
    listeners/tcp_listener.c)

set(HAWKTRACER_CORE_SOURCES
    alloc.c
    bag.c
    command_line_parser.c
    event_id_provider.cpp
    event_utils.c
    events.c
    feature_cached_string.c
    feature_callstack.c
    global_timeline.cpp
    hash_map.c
    init.c
    listener_buffer.c
    monotonic_clock.cpp
    mutex.cpp
    registry.c
    scoped_tracepoint.c
    stack.c
    system_info.c
    task_scheduler.c
    tcp_server.cpp
    thread.c
    timeline.c
    timeline_listener.c)

macro(INCLUDE_FEATURE FEATURE_NAME FEATURE_HEADER)
    if (HT_PLATFORM_FEATURE_${FEATURE_NAME}_ENABLED)
        list(APPEND HAWKTRACER_CORE_HEADERS ${FEATURE_HEADER})
        list(APPEND HAWKTRACER_CORE_SOURCES "${HT_PLATFORM_FEATURE_${FEATURE_NAME}_SOURCE}")
    endif()
endmacro(INCLUDE_FEATURE)

INCLUDE_FEATURE(CPU_USAGE include/hawktracer/cpu_usage.h)
INCLUDE_FEATURE(MEMORY_USAGE include/hawktracer/memory_usage.h)
INCLUDE_FEATURE(ALLOC_HOOKS include/hawktracer/alloc_hooks.h)

if (CMAKE_USE_PTHREADS_INIT)
    set(HT_USE_PTHREADS ON)
    list(APPEND HAWKTRACER_CORE_HEADERS include/hawktracer/posix_mapped_tracepoint.h)
    list(APPEND HAWKTRACER_CORE_SOURCES posix_mapped_tracepoint.c)
endif()

if (BUILD_STATIC_LIB)
    set(HT_COMPILE_STATIC ON)
endif (BUILD_STATIC_LIB)

configure_file(include/hawktracer/ht_config.h.in include/hawktracer/ht_config.h)

set(HAWKTRACER_LIB_SOURCES ${HAWKTRACER_LISTENERS_SOURCES} ${HAWKTRACER_CORE_SOURCES})
set(HAWKTRACER_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_BINARY_DIR}/include")

add_library(hawktracer ${HAWKTRACER_LIB_TYPE} ${HAWKTRACER_LIB_SOURCES})

if(ENABLE_POSITION_INDEPENDENT_CODE)
    set_property(TARGET hawktracer PROPERTY POSITION_INDEPENDENT_CODE ON)
endif(ENABLE_POSITION_INDEPENDENT_CODE)

target_include_directories(hawktracer PUBLIC
    "$<BUILD_INTERFACE:${HAWKTRACER_INCLUDE_DIRS}>"
    $<INSTALL_INTERFACE:include>)
target_compile_definitions(hawktracer PRIVATE -DHT_COMPILE_SHARED_EXPORT)
target_link_libraries(hawktracer INTERFACE ${CMAKE_THREAD_LIBS_INIT})

install(TARGETS hawktracer
    EXPORT HawkTracerTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    COMPONENT coredevelopment)

install(DIRECTORY include/hawktracer
    DESTINATION include
    COMPONENT coredevelopment
    PATTERN "ht_config.h.in" EXCLUDE)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/include/hawktracer/ht_config.h
    DESTINATION include/hawktracer/
    COMPONENT coredevelopment)
install(FILES
    include/hawktracer.h
    DESTINATION include/
    COMPONENT coredevelopment)
