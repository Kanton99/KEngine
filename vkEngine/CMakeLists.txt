cmake_minimum_required(VERSION 4.3)
set(CMAKE_CXX_STANDARD 23)
project("vkEngine")

find_package(Vulkan REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_package(VulkanHeaders CONFIG)
find_package(fastgltf CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_library(
    ${PROJECT_NAME}
    SHARED
    "src/engine.cpp"
)
add_library(KEngine::vkEngine ALIAS ${PROJECT_NAME})

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_definitions(
    Vulkan::Headers
    INTERFACE VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
)

target_link_libraries(${PROJECT_NAME} PUBLIC
  glm::glm
  Vulkan::Vulkan
  GPUOpen::VulkanMemoryAllocator
  fastgltf::fastgltf
)

