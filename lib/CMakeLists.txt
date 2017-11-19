set(HAWKTRACER_LIB_HEADERS
    include/hawktracer/base_types.h
    include/hawktracer/events.h
    include/hawktracer/macros.h
    include/hawktracer/monotonic_clock.h)

set(HAWKTRACER_LIB_SOURCES
    alloc.c
    callstack_base_timeline.cpp
    callstack_timeline.c
    event_id_provider.cpp
    events.c
    init.c
    monotonic_clock.cpp
    mutex.cpp
    thread.cpp
    timeline.cpp
    timeline_listener.cpp
    timeline_registry.cpp)

include_directories(include)

add_library(hawktracer ${HAWKTRACER_LIB_SOURCES})
