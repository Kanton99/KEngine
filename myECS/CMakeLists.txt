cmake_minimum_required(VERSION 3.30)
set(CMAKE_CXX_STANDARD 20)
project("myECS")

add_executable(${PROJECT_NAME} "myECS.hpp" "myECS.cpp")