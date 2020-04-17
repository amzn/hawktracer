include(CheckCXXCompilerFlag)

set(HAWKTRACER_LIB_HEADERS
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
    include/hawktracer/timeline_listener.h

    # listeners
    include/hawktracer/listeners/file_dump_listener.h)

set(HAWKTRACER_CPP_LIB_SOURCES
    event_id_provider.cpp
    global_timeline.cpp
    monotonic_clock.cpp
    mutex.cpp)

if(FORCE_C_COMPILATION)
    # setting LANGUAGE property for gcc doesn't seem to be enough as it
    # only sets -std=gnu99, but that doesn't actually force compiler to
    # use C. So we set the language explicitly in the compiler.
    check_cxx_compiler_flag("-x c" COMPILER_SUPPORTS_LANGUAGE_SELECTION)
    if(COMPILER_SUPPORTS_LANGUAGE_SELECTION)
        set_source_files_properties(${HAWKTRACER_CPP_LIB_SOURCES} PROPERTIES COMPILE_FLAGS "-x c")
    endif()
    set_source_files_properties(${HAWKTRACER_CPP_LIB_SOURCES} PROPERTIES LANGUAGE C)
endif()

set(HAWKTRACER_LIB_SOURCES
    alloc.c
    bag.c
    command_line_parser.c
    event_utils.c
    events.c
    feature_cached_string.c
    feature_callstack.c
    hash_map.c
    init.c
    listener_buffer.c
    registry.c
    scoped_tracepoint.c
    stack.c
    system_info.c
    task_scheduler.c
    thread.c
    timeline.c
    timeline_listener.c
    ${HAWKTRACER_CPP_LIB_SOURCES}

    # listeners
    listeners/file_dump_listener.c)

macro(INCLUDE_FEATURE FEATURE_NAME FEATURE_HEADER)
    if (HT_PLATFORM_FEATURE_${FEATURE_NAME}_ENABLED)
        list(APPEND HAWKTRACER_LIB_HEADERS ${FEATURE_HEADER})
        list(APPEND HAWKTRACER_LIB_SOURCES "${HT_PLATFORM_FEATURE_${FEATURE_NAME}_SOURCE}")
    endif()
endmacro(INCLUDE_FEATURE)

INCLUDE_FEATURE(CPU_USAGE include/hawktracer/cpu_usage.h)
INCLUDE_FEATURE(MEMORY_USAGE include/hawktracer/memory_usage.h)
INCLUDE_FEATURE(ALLOC_HOOKS include/hawktracer/alloc_hooks.h)

if (BUILD_STATIC_LIB)
    set(HT_COMPILE_STATIC ON)
endif (BUILD_STATIC_LIB)

if (ENABLE_TCP_LISTENER)
    if (NOT ENABLE_THREADS)
        message(FATAL_ERROR "Threads must be enabled for TCP listener")
    endif()
    set(HT_ENABLE_TCP_LISTENER ON)
    list(APPEND HAWKTRACER_LIB_SOURCES listeners/tcp_listener.c tcp_server.cpp)
    list(APPEND HAWKTRACER_LIB_HEADERS include/hawktracer/listeners/tcp_listener.h)
endif()

if (ENABLE_THREADS)
    set(HT_ENABLE_THREADS ON)

    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)

    if (CMAKE_USE_PTHREADS_INIT)
        set(HT_USE_PTHREADS ON)
        list(APPEND HAWKTRACER_LIB_HEADERS include/hawktracer/posix_mapped_tracepoint.h)
        list(APPEND HAWKTRACER_LIB_SOURCES posix_mapped_tracepoint.c)
    endif()
endif()

configure_file(include/hawktracer/ht_config.h.in include/hawktracer/ht_config.h)

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
