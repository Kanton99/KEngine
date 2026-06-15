# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
include(FetchContent)
cmake_minimum_required(VERSION 4.3)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SDL_VIDEO_VULKAN ON CACHE BOOL "" FORCE)
set(SDL_VIDEO_WAYLAND ON CACHE BOOL "" FORCE)

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

# Compile shaders during project compilation
function(add_slang_shader_target TARGET)
  cmake_parse_arguments("SHADER" "" "" "SOURCES" ${ARGN})
  set(SHADER_DIR ${CMAKE_CURRENT_LIST_DIR}/shaders)
  set(ENTRY_POINTS -entry vertMain -entry fragMain)
  add_custom_command(
    OUTPUT ${SHADER_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_DIR}
  )
  add_custom_command(
    OUTPUT ${SHADER_DIR}/slang.spv
    COMMAND ${SLANGC_EXECUTABLE} ${SHADER_SOURCES}
      -target spirv
      -profile spirv_1_4
      -emit-spirv-directly
      -fvk-use-entrypoint-name
      ${ENTRY_POINTS}
      -o slang.spv
    WORKING_DIRECTORY ${SHADER_DIR}
    DEPENDS ${SHADER_DIR} ${SHADER_SOURCES}
    COMMENT "Compiling Slang Shaders"
    VERBATIM
  )
  add_custom_target(${TARGET} DEPENDS ${SHADER_DIR}/slang.spv)
endfunction()

add_slang_shader_target(shaders SOURCES $ {SHADER_SLANG_SOURCES})
add_dependencies(${PROJECT} shaders)

# Include sub-projects.
add_subdirectory("vkEngine")
add_subdirectory("main")
# add_subdirectory("myECS")
# add_dependencies(vkEngine copy_assets)

# target_link_libraries(KEngine PRIVATE KEngine::vkEngine)
# target_include_directories(KEngine PRIVATE vkEngine/include)
