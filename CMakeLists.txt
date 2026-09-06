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
FetchContent_Declare(vmahpp URL
																					https://github.com/YaaZ/VulkanMemoryAllocator-Hpp/releases/download/v3.3.0%2B3/VulkanMemoryAllocator-Hpp-3.3.0.tar.gz
																					URL_HASH
																					SHA256=8c2a0573babe1f86f3241d12b4c9df40bb944649b7f4239c1c611797f8af6cd5
																				)
# Copy from the asset list on the release page)
FetchContent_MakeAvailable(vmahpp)

project("KEngine")
# add_executable(${PROJECT_NAME} "main.cpp")

add_custom_target(
				copy_assets
				COMMAND
								${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/resources
								${CMAKE_CURRENT_BINARY_DIR}/resources
)

# Compile shaders during project compilation
function(add_slang_shader_target target)
	cmake_parse_arguments("SHADER" "" "" "SOURCES" ${ARGN})
	set(shaders_dir ${CMAKE_CURRENT_LIST_DIR}/resources/shaders)
	set(entry_points -entry vertMain -entry fragMain)
	add_custom_command(
				OUTPUT ${shaders_dir}
				COMMAND ${CMAKE_COMMAND} -E make_directory ${shaders_dir}
		)
	add_custom_command(
				OUTPUT ${shaders_dir}/slang.spv
				COMMAND slangc ${SHADER_SOURCES}
						-target spirv
						-profile spirv_1_4
						-emit-spirv-directly
						-fvk-use-entrypoint-name
						${entry_points}
						-o slang.spv
				WORKING_DIRECTORY ${shaders_dir}
				DEPENDS ${shaders_dir} ${SHADER_SOURCES}
				COMMENT "Compiling Slang Shaders"
				VERBATIM
		)
	add_custom_target(${target} DEPENDS ${shaders_dir}/slang.spv)
endfunction()

add_slang_shader_target(
		shaders
		SOURCES /home/antonv/dev/KEngine/resources/shaders/base.slang
)

# Include sub-projects.
add_subdirectory("vkEngine")
add_subdirectory("main")
# add_subdirectory("myECS")
# add_dependencies(vkEngine copy_assets)
add_dependencies(vkEngine shaders)

# target_link_libraries(KEngine PRIVATE KEngine::vkEngine)
# target_include_directories(KEngine PRIVATE vkEngine/include)
