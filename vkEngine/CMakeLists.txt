cmake_minimum_required(VERSION 3.30)
set(CMAKE_CXX_STANDARD 20)
project("vkEngine")

find_package(Vulkan REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_package(unofficial-vulkan-memory-allocator-hpp CONFIG REQUIRED)
find_package(VulkanHeaders CONFIG)
find_package(vk-bootstrap CONFIG REQUIRED)
find_package(fastgltf CONFIG REQUIRED)

add_library(${PROJECT_NAME} STATIC 
    "src/vkEngine.cpp"
    "src/utils.cpp"
    "src/pipeline_builder.cpp"
    "src/init.cpp"
    "src/infoCreator.cpp"
    "src/gltfLoader.cpp"
    "src/objLoader.cpp"
)
add_library(lib::vkEngine ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC
  ${PROJECT_SOURCE_DIR}/include
)

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

target_link_libraries(${PROJECT_NAME} PRIVATE fastgltf::fastgltf)

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
        COMMAND $ENV{VULKAN_SDK}/bin/glslangValidator -V ${SHADER} -o ${COMPILED_SHADER}
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
