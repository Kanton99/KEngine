# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required(VERSION 3.30)
set(CMAKE_CXX_STANDARD 20)
project("vkEngine")


find_package(SDL3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(unofficial-vulkan-memory-allocator-hpp CONFIG REQUIRED)
find_package(VulkanHeaders CONFIG)
find_package(vk-bootstrap CONFIG REQUIRED)

add_executable(${PROJECT_NAME} main.cpp "vkEngine.h" "vkEngine.cpp" "util_structs.hpp")

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)

target_compile_definitions(Vulkan::Headers INTERFACE
	VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
)
target_link_libraries(${PROJECT_NAME} PRIVATE vk-bootstrap::vk-bootstrap vk-bootstrap::vk-bootstrap-compiler-warnings ${CMAKE_DL_LIBS})
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan Vulkan::Headers)
target_link_libraries(${PROJECT_NAME} PRIVATE unofficial::VulkanMemoryAllocator-Hpp::VulkanMemoryAllocator-Hpp)
