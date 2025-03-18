
cmake_minimum_required(VERSION 3.30)
set(CMAKE_CXX_STANDARD 20)

find_package(SDL3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

project("main")
add_executable(${PROJECT_NAME} "main.cpp")

target_link_libraries(${PROJECT_NAME} PUBLIC lib::vkEngine)

