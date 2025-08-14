cmake_minimum_required(VERSION 3.30)
set(CMAKE_CXX_STANDARD 23)
project("myECS")

add_library(${PROJECT_NAME} SHARED "src/SystemManager.cpp")
add_library(KEngine::myECS ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
