cmake_minimum_required(VERSION 3.10)
project(MAI_SEM3_OS)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(main main.cpp)

add_executable(child child.cpp)