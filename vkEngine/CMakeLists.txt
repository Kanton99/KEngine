cmake_minimum_required(VERSION 3.30)
set(CMAKE_CXX_STANDARD 20)
project("vkEngine")

find_package(SDL3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_package(unofficial-vulkan-memory-allocator-hpp CONFIG REQUIRED)
find_package(VulkanHeaders CONFIG)
find_package(vk-bootstrap CONFIG REQUIRED)

add_executable(
    ${PROJECT_NAME}
    main.cpp
    "vkEngine.hpp"
    "vkEngine.cpp"
    "util_structs.hpp"
    "utils.hpp"
    "utils.cpp"
    "pipeline_builder.hpp"
    "pipeline_builder.cpp"
    "init.hpp"
    "init.cpp"
    "infoCreator.hpp"
    "infoCreator.cpp"
    "infoCreator.hpp"
)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)

target_compile_definitions(
    Vulkan::Headers
    INTERFACE VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
)
target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
        vk-bootstrap::vk-bootstrap
        vk-bootstrap::vk-bootstrap-compiler-warnings
        ${CMAKE_DL_LIBS}
)
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)
target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
        GPUOpen::VulkanMemoryAllocator
        unofficial::VulkanMemoryAllocator-Hpp::VulkanMemoryAllocator-Hpp
)
# Set paths for shaders
set(SHADER_DIR ${CMAKE_SOURCE_DIR}/resources/shaders)
set(SHADER_BINARY_DIR ${CMAKE_SOURCE_DIR}/resources/shaders/compiled)

# Find all GLSL shader files
file(GLOB SHADERS "${SHADER_DIR}/*.vert" "${SHADER_DIR}/*.frag" "${SHADER_DIR}/*.comp")

set(COMPILED_SHADERS)

foreach(SHADER ${SHADERS})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    set(COMPILED_SHADER ${SHADER_BINARY_DIR}/${SHADER_NAME}.spv)

    add_custom_command(
        OUTPUT ${COMPILED_SHADER}
        COMMAND $ENV{VULKAN_SDK}/bin/glslangValidator.exe -V ${SHADER} -o ${COMPILED_SHADER}
        DEPENDS ${SHADER}
        COMMENT "Compiling GLSL shader: ${SHADER_NAME}"
        VERBATIM
    )

    list(APPEND COMPILED_SHADERS ${COMPILED_SHADER})
endforeach()

# Ensure the shaders are compiled before building your main project
add_custom_target(compile_shaders DEPENDS ${COMPILED_SHADERS})

# Make sure your executable depends on the compiled shaders
add_dependencies(${PROJECT_NAME} compile_shaders)
