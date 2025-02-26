# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required(VERSION 3.30)
set(CMAKE_CXX_STANDARD 20)

project(KEngine)


add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/resources ${CMAKE_CURRENT_BINARY_DIR}/resources
)


# Include sub-projects.
add_subdirectory ("vkEngine")
add_dependencies(vkEngine copy_assets)
