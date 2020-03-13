# https://www.npmjs.com/package/cmake-js#general
if (CMAKE_JS_SRC)
    add_library(cmake_js STATIC ${CMAKE_JS_SRC})
else()
    add_library(cmake_js INTERFACE)
endif()
target_include_directories(cmake_js INTERFACE SYSTEM ${CMAKE_JS_INC})
target_link_libraries(cmake_js ${CMAKE_JS_LIB})

# https://www.npmjs.com/package/cmake-js#n-api-and-node-addon-api
add_library(node_addon_api INTERFACE)
if (NOT NODE_RUNTIME)
    find_program(NODE_RUNTIME NAMES node HINTS /usr)
endif()
execute_process(COMMAND ${NODE_RUNTIME} -p "require('node-addon-api').include"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE NODE_ADDON_API_DIR
        )
if (NOT NODE_ADDON_API_DIR)
    message(FATAL_ERROR "Failed to find node-addon-api headers with node command \"${NODE_RUNTIME}\"")
endif()
string(REPLACE "\n" "" NODE_ADDON_API_DIR "${NODE_ADDON_API_DIR}")
string(REPLACE "\"" "" NODE_ADDON_API_DIR "${NODE_ADDON_API_DIR}")
target_include_directories(node_addon_api INTERFACE SYSTEM ${NODE_ADDON_API_DIR})
target_link_libraries(node_addon_api INTERFACE cmake_js)

# Supports Node version >= 12.11.0 https://nodejs.org/api/n-api.html#n_api_n_api_version_matrix
target_compile_definitions(node_addon_api INTERFACE NAPI_VERSION=5)

if (ENABLE_CLIENT)
    add_library(nodejs_bindings_client SHARED
            hawktracer_client_nodejs.cpp
            hawktracer_client_nodejs.hpp
            client_context.cpp
            client_context.hpp)
    set_target_properties(nodejs_bindings_client PROPERTIES PREFIX "" OUTPUT_NAME hawk_tracer_client SUFFIX .node)
    target_link_libraries(nodejs_bindings_client PUBLIC node_addon_api hawktracer_parser hawktracer_client_utils)

    # cmake-js specifies CMAKE_LIBRARY_OUTPUT_DIRECTORY.
    add_custom_target(make_directory_for_nodejs_bindings_client
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
    add_dependencies(nodejs_bindings_client make_directory_for_nodejs_bindings_client)
endif ()
