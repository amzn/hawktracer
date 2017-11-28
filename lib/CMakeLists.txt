set(HAWKTRACER_LIB_HEADERS
    include/hawktracer/base_types.h
    include/hawktracer/events.h
    include/hawktracer/macros.h
    include/hawktracer/monotonic_clock.h)

set(HAWKTRACER_LIB_SOURCES
    alloc.c
    bag.c
    base_timeline.cpp
    callstack_base_timeline.c
    callstack_timeline.c
    event_id_provider.cpp
    events.c
    global_callstack_timeline.cpp
    init.c
    monotonic_clock.cpp
    mutex.cpp
    stack.c
    thread.cpp
    timeline.cpp
    timeline_klass.cpp
    timeline_listener.c
    timeline_registry.c)

include_directories(include)

add_library(hawktracer ${HAWKTRACER_LIB_SOURCES})
