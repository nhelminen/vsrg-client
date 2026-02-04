cmake_minimum_required(VERSION 3.28)
project(VSRG_Mania VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

get_filename_component(PARENT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
set(ENGINE_INCLUDE_DIR "${PARENT_DIR}/include")

include_directories(${ENGINE_INCLUDE_DIR})

file(GLOB_RECURSE PLUGIN_SOURCES CONFIGURE_DEPENDS
    "src/*.[ch]pp"
    "src/*.[ch]"
)

add_library(${PROJECT_NAME} SHARED ${PLUGIN_SOURCES})

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        PREFIX ""
        SUFFIX ".dll"
    )
elseif(UNIX)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        PREFIX ""
        SUFFIX ".so"
    )
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugins"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugins"
)