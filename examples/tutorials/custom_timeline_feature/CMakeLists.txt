add_executable(custom-timeline-feature
    push-counter-feature.c
    hawktracer-custom-timeline-feature.c)
target_link_libraries(custom-timeline-feature hawktracer)
