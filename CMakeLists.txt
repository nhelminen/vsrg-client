cmake_minimum_required(VERSION 3.28)
set(PROJECT_NAME "vsrg-client")
project(${PROJECT_NAME} VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(OpenGL REQUIRED)

add_subdirectory("3rdparty")
set(BUILD_SHARED_LIBS TRUE)

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
	"src/*.[ch]pp"
	"src/*.[ch]"
	"3rdparty/glad/src/glad.c"
)
list(FILTER ENGINE_SOURCES EXCLUDE REGEX ".*main\\.cpp$")

add_library(vsrg-engine SHARED ${ENGINE_SOURCES})

if(WIN32)
    set_target_properties(vsrg-engine PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS ON
    )
endif()
target_link_libraries(vsrg-engine PUBLIC "3rdparty" OpenGL::GL)

target_compile_definitions(vsrg-engine PUBLIC 
    VSRG_PROJECT_ROOT="${CMAKE_SOURCE_DIR}/"
)

target_include_directories(vsrg-engine PUBLIC 
    ${PROJECT_SOURCE_DIR}/include
)

add_executable(${PROJECT_NAME} "src/main.cpp")
target_link_libraries(${PROJECT_NAME} PRIVATE vsrg-engine)

if(EXISTS "${PROJECT_SOURCE_DIR}/plugins")
	add_subdirectory("plugins")
endif()

if(EXISTS "${PROJECT_SOURCE_DIR}/assets")
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${PROJECT_SOURCE_DIR}/assets"
		"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets"
	)
endif()