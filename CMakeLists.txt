cmake_minimum_required(VERSION 3.1)

project(homework5_7)

set(homework5_7_SRC main.cpp vec3.cpp)
set(homework5_7_HDR Factory.h header.h Shapes.h vec3.h)

add_executable(${PROJECT_NAME} ${homework5_7_SRC} ${homework5_7_HDR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 /std:c++14")
