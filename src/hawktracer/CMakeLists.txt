if(UNIX)
    set(platform unix)
else()
    message(ERROR "Unsupported platform ${CMAKE_SYSTEM_NAME}")
endif(UNIX)

set(hawktracer_PUBLIC_HEADERS
    Action.h
    CircularBuffer.h
    config.h
    TCPClient.h
    TCPServer.h
    Timeline.h
    TimelineNetworkListener.h
    TracepointTimeline.h
    Vector.h)

add_library(hawktracer STATIC
    Action.cpp
    TCPClient.cpp
    TCPServer.cpp
    Timeline.cpp
    TimelineNetworkListener.cpp
    platform/${platform}/HRClock.cpp)

set_target_properties(hawktracer PROPERTIES COMPILE_FLAGS " -Wall -std=c++0x")

install(
    TARGETS hawktracer
    DESTINATION "lib")
install(FILES ${hawktracer_PUBLIC_HEADERS} DESTINATION "include/hawktracer/")
        
