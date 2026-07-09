cmake_minimum_required(VERSION 3.0)
project(week5_GLSL_Deep_Dive)

set(CMAKE_CXX_STANDARD 17)

add_executable(week5 main.cpp)
target_include_directories(week5 PRIVATE ${CMAKE_SOURCE_DIR}/..)
target_link_libraries(week5 PRIVATE glfw GLEW GL)
