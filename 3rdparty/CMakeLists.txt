add_subdirectory("SDL" EXCLUDE_FROM_ALL)

add_subdirectory("glm")
set(GLM_ENABLE_CXX_20 ON)

add_subdirectory("freetype")

add_library("3rdparty" INTERFACE)

target_include_directories("3rdparty" INTERFACE "glad/include" glm::glm freetype)

target_link_libraries("3rdparty" INTERFACE SDL3::SDL3 glm::glm freetype)