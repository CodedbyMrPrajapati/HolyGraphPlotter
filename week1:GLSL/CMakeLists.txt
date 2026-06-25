cmake_minimum_required(VERSION 3.15)

project(EntityManager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

option(ENABLE_SANITIZERS "Enable Address and Undefined Behavior Sanitizers" ON)

add_executable(
    EntityManager
    Entity_Manager.cpp
)

target_compile_options(
    EntityManager
    PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

if(ENABLE_SANITIZERS)
    if(NOT MSVC)
        target_compile_options(
            EntityManager
            PRIVATE
            -fsanitize=address,undefined
            -fno-omit-frame-pointer
        )

        target_link_options(
            EntityManager
            PRIVATE
            -fsanitize=address,undefined
        )
    endif()
endif()