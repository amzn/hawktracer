add_subdirectory(hawktracer)

if(NOT DISABLE_CLIENT)
    add_subdirectory(profiler)
endif()
