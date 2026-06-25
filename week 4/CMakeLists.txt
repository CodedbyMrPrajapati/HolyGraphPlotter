cmake_minimum_required(VERSION 3.15)
project(Week4Rubik CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
add_executable(rubik main.cpp)

# Try finding system packages, fallback to manual links if using MinGW/GCC on Windows
find_package(OpenGL REQUIRED)
find_package(GLEW QUIET)
find_package(glfw3 QUIET)

if(GLEW_FOUND AND glfw3_FOUND)
    target_link_libraries(rubik PRIVATE OpenGL::GL GLEW::GLEW glfw)
else()
    # MinGW/GCC fallback links
    target_link_libraries(rubik PRIVATE glew32 glfw3 opengl32 gdi32)
endif()
