set(HAWKTRACER_LIB_HEADERS
    include/hawktracer/base_types.h
    include/hawktracer/events.h
    include/hawktracer/macros.h
    include/hawktracer/monotonic_clock.h)

set(HAWKTRACER_LISTENERS_SOURCES
    listeners/file_dump_listener.c)

set(HAWKTRACER_LIB_SOURCES
    alloc.c
    bag.c
    event_id_provider.cpp
    events.c
    feature_cached_string.c
    feature_callstack.c
    global_timeline.cpp
    init.c
    monotonic_clock.cpp
    mutex.cpp
    registry.c
    stack.c
    thread.cpp
    timeline.c
    timeline_listener.c)

include_directories(include)

add_library(hawktracer ${HAWKTRACER_LIB_SOURCES} ${HAWKTRACER_LISTENERS_SOURCES})
