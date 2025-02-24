# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
# set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

# if (MSVC)
#     execute_process(
#         COMMAND cmd /c "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvarsall.bat x64 && set"
#         OUTPUT_VARIABLE ENV_VARS
#     )
#
#     string(REGEX MATCHALL "[^\r\n]+" ENV_VARS_LIST "${ENV_VARS}")
#     foreach (env_var ${ENV_VARS_LIST})
#         string(REGEX MATCH "([^=]+)=(.*)" _ ${env_var})
#         set(ENV{${CMAKE_MATCH_1}} ${CMAKE_MATCH_2})
#     endforeach()
# endif()
cmake_minimum_required(VERSION 3.30)
set(CMAKE_CXX_STANDARD 20)

project(KEngine)


add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/resources ${CMAKE_CURRENT_BINARY_DIR}/resources
)


# Include sub-projects.
add_subdirectory ("vkEngine")
add_subdirectory ("myECS")
add_dependencies(vkEngine copy_assets)
