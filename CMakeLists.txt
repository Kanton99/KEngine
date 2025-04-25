# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required(VERSION 3.30)
set(CMAKE_CXX_STANDARD 20)

project("KEngine")
# add_executable(${PROJECT_NAME} "main.cpp")

add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/resources ${CMAKE_CURRENT_BINARY_DIR}/resources
)

# Include sub-projects.
add_subdirectory ("vkEngine")
add_subdirectory("main")
add_subdirectory("myECS")
# add_dependencies(vkEngine copy_assets)

# target_link_libraries(KEngine PRIVATE KEngine::vkEngine)
# target_include_directories(KEngine PRIVATE vkEngine/include)
