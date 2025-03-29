cmake_minimum_required(VERSION 3.10)

project(route_planner)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(routeplanner src/main.cpp src/storage.cpp)
