# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
include(FetchContent)
cmake_minimum_required(VERSION 4.3)
set(CMAKE_CXX_STANDARD 23)


FetchContent_Declare(
  SDL3
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG release-3.4.2
)

FetchContent_MakeAvailable(SDL3)
project("KEngine")
# add_executable(${PROJECT_NAME} "main.cpp")

add_custom_target(
    copy_assets
    COMMAND
        ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/resources
        ${CMAKE_CURRENT_BINARY_DIR}/resources
)

# Include sub-projects.
add_subdirectory("vkEngine")
add_subdirectory("main")
# add_subdirectory("myECS")
# add_dependencies(vkEngine copy_assets)

# target_link_libraries(KEngine PRIVATE KEngine::vkEngine)
# target_include_directories(KEngine PRIVATE vkEngine/include)
