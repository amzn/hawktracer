set(HAWKTRACER_LISTENER_HEADERS
    include/hawktracer/listeners/file_dump_listener.h
    include/hawktracer/listeners/tcp_listener.h)

set(HAWKTRACER_CORE_HEADERS
    include/hawktracer/alloc.h
    include/hawktracer/bag.h
    include/hawktracer/base_types.h
    include/hawktracer/callstack_scoped_tracepoint.h
    include/hawktracer/callstack_scoped_tracepoint.hpp
    include/hawktracer/core_events.h
    include/hawktracer/event_id_provider.h
    include/hawktracer/event_macros.h
    include/hawktracer/events.h
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
    include/hawktracer/stack.h
    include/hawktracer/thread.h
    include/hawktracer/timeline.h
    include/hawktracer/timeline_listener.h)

set(HAWKTRACER_LIB_HEADERS ${HAWKTRACER_LISTENER_HEADERS} ${HAWKTRACER_CORE_HEADERS})

set(HAWKTRACER_LISTENERS_SOURCES
    listeners/file_dump_listener.c
    listeners/tcp_listener.cpp)

set(HAWKTRACER_CORE_SOURCES
    alloc.c
    bag.c
    event_id_provider.cpp
    events.c
    feature_cached_string.c
    feature_callstack.c
    global_timeline.cpp
    init.c
    listener_buffer.c
    monotonic_clock.cpp
    mutex.cpp
    registry.c
    stack.c
    tcp_server.cpp
    thread.cpp
    timeline.c
    timeline_listener.c)

set(HAWKTRACER_LIB_SOURCES ${HAWKTRACER_LISTENERS_SOURCES} ${HAWKTRACER_CORE_SOURCES})

include_directories(include)

# We need to compile sources 2 times - the reason why we can't re-use object files
# is because for shared lib they'll be compiled with -fPIC, which is not needed
# (and might affect a performance) for static libraries.
set(HAWKTRACER_LIB_INSTALL_TARGETS)
if(ENABLE_SHARED_LIB)
    add_library(hawktracer SHARED ${HAWKTRACER_LIB_SOURCES})
    target_compile_definitions(hawktracer PRIVATE -DHT_COMPILE_SHARED_EXPORT)
    list(APPEND HAWKTRACER_LIB_INSTALL_TARGETS hawktracer)
endif(ENABLE_SHARED_LIB)
if(ENABLE_STATIC_LIB)
    add_library(hawktracer_static STATIC ${HAWKTRACER_LIB_SOURCES})
    list(APPEND HAWKTRACER_LIB_INSTALL_TARGETS hawktracer_static)
endif(ENABLE_STATIC_LIB)

install(TARGETS ${HAWKTRACER_LIB_INSTALL_TARGETS}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

install(DIRECTORY include/hawktracer DESTINATION include)
