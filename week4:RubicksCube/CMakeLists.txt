cmake_minimum_required(VERSION 3.15)
project(Week3Cube CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(cube
    main.cpp
    ../gl_framework.cpp
    ../shader_util.cpp
)

find_package(OpenGL REQUIRED)
find_package(GLEW QUIET)
find_package(glfw3 QUIET)

if(GLEW_FOUND AND glfw3_FOUND)
    target_link_libraries(cube PRIVATE OpenGL::GL GLEW::GLEW glfw)
else()
    if(WIN32)
        target_link_libraries(cube PRIVATE glew32 glfw3 opengl32 gdi32)
    else()
        target_link_libraries(cube PRIVATE ${OPENGL_gl_LIBRARY} -lGLEW -lglfw)
    endif()
endif()
