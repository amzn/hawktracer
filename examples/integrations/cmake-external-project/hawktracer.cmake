# HawkTracer requires threads
find_package(Threads REQUIRED)
# include ExternalProject module
include(ExternalProject)

# This is optonal, only if you want to build HawkTracer as static lib
set(HT_BUILD_STATIC ON)

# Add new project. You can find more options in CMake documentation:
# https://cmake.org/cmake/help/v3.0/module/ExternalProject.html
ExternalProject_Add(HawkTracerExt
  GIT_REPOSITORY https://github.com/amzn/hawktracer.git
  GIT_TAG master    # Git ref of the project. It could be commit hash, branch name or tag.
  INSTALL_COMMAND "" # we don't want to install the library, just build it
  CMAKE_ARGS "-DBUILD_STATIC_LIB=${HT_BUILD_STATIC}"
  )

# If we build library statically, the output file has different name
if(HT_BUILD_STATIC)
  set(HT_LIB_FILE_NAME "${CMAKE_STATIC_LIBRARY_PREFIX}hawktracer${CMAKE_STATIC_LIBRARY_SUFFIX}")
else()
  set(HT_LIB_FILE_NAME "${CMAKE_SHARED_LIBRARY_PREFIX}hawktracer${CMAKE_SHARED_LIBRARY_SUFFIX}")
endif()

# Get some properties from the project
ExternalProject_Get_Property(HawkTracerExt source_dir)
ExternalProject_Get_Property(HawkTracerExt binary_dir)

# This is workaround for CMake error - included directories must exist during
# the configuration step. But CMake downloads external projects on build step,
# so we need to create empty directories manually to trick CMake.
set(HAWKTRACER_INCLUDE_DIRS ${source_dir}/lib/include ${binary_dir}/lib/include/)
file(MAKE_DIRECTORY ${HAWKTRACER_INCLUDE_DIRS})

# Set library path - MSVC is slightly different here. Let us know if there are
# other special cases.
set(HAWKTRACER_LIBS_DIRS ${binary_dir}/lib)
if(MSVC)
    set(HAWKTRACER_LIBRARY_PATH "${HAWKTRACER_LIBS_DIRS}/$(Configuration)/${HT_LIB_FILE_NAME}")
else()
    set(HAWKTRACER_LIBRARY_PATH "${HAWKTRACER_LIBS_DIRS}/${HT_LIB_FILE_NAME}")
endif()

add_library(hawktracer UNKNOWN IMPORTED)
set_target_properties(hawktracer PROPERTIES
  IMPORTED_LOCATION ${HAWKTRACER_LIBRARY_PATH}
  INTERFACE_INCLUDE_DIRECTORIES "${HAWKTRACER_INCLUDE_DIRS}"
  INTERFACE_LINK_LIBRARIES "${CMAKE_THREAD_LIBS_INIT}")

add_dependencies(hawktracer HawkTracerExt)
