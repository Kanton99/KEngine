include(FetchContent)
set(CMAKE_CXX_STANDARD 23)

find_package(glm CONFIG REQUIRED)

project("main")
add_executable(${PROJECT_NAME} "main.cpp")

target_link_libraries(${PROJECT_NAME} PUBLIC KEngine::vkEngine SDL3::SDL3)
