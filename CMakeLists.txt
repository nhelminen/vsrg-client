cmake_minimum_required(VERSION 3.28)
set(PROJECT_NAME "vsrg")
project(${PROJECT_NAME} VERSION 0.0.0)

set(BUILD_SHARED_LIBS FALSE)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

find_package(OpenGL REQUIRED)

add_subdirectory("3rdparty")

include_directories(${PROJECT_SOURCE_DIR}/src)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
	"src/*.[ch]pp"
	"src/*.[ch]"
	"3rdparty/glad/src/glad.c"
)
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE "libraries")