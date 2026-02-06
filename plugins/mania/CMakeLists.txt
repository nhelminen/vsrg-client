cmake_minimum_required(VERSION 3.28)
project(VSRG_Mania VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

get_filename_component(PARENT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE PLUGIN_SOURCES CONFIGURE_DEPENDS
    "src/*.[ch]pp"
    "src/*.[ch]"
)

add_library(${PROJECT_NAME} SHARED ${PLUGIN_SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE vsrg-engine)

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
    set_target_properties(${PROJECT_NAME} PROPERTIES
        BUILD_RPATH "\$ORIGIN/.."
        INSTALL_RPATH "\$ORIGIN/.."
    )
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugins"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/plugins"
)