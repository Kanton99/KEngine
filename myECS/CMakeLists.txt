cmake_minimum_required(VERSION 3.30)
set(CMAKE_CXX_STANDARD 20)
project("myECS")

add_library(
  ${PROJECT_NAME} SHARED "src/SystemManager.cpp" "src/ComponentManager.cpp"
                         "src/EntityManager.cpp")
add_library(KEngine::myECS ALIAS ${PROJECT_NAME})

find_package(stduuid CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE stduuid)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
