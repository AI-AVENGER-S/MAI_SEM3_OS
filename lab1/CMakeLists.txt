cmake_minimum_required(VERSION 3.10)
project(MAI_SEM2_OS)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(main main.cpp)

add_executable(child src/child.cpp src/solution.cpp)