cmake_minimum_required(VERSION 3.15)
project(Week7ExprDSL CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic)

add_executable(expr_demo
    main.cpp
)
