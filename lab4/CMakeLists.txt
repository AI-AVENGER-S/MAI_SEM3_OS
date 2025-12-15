cmake_minimum_required(VERSION 3.10)
project(OS_Lab)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(impl1_static STATIC lib/impl1.cpp)
add_library(impl2_static STATIC lib/impl2.cpp)

add_library(impl1_shared SHARED lib/impl1.cpp)
add_library(impl2_shared SHARED lib/impl2.cpp)

set_target_properties(impl1_shared PROPERTIES 
    OUTPUT_NAME "impl1"
    SUFFIX ".so"
)

set_target_properties(impl2_shared PROPERTIES 
    OUTPUT_NAME "impl2"
    SUFFIX ".so"
)

add_executable(main_static app/main_static.cpp)
target_link_libraries(main_static impl1_static)

add_executable(main_dynamic app/main_dynamic.cpp)
target_link_libraries(main_dynamic ${CMAKE_DL_LIBS})

add_custom_command(TARGET main_dynamic POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:impl1_shared>
        $<TARGET_FILE_DIR:main_dynamic>/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:impl2_shared>
        $<TARGET_FILE_DIR:main_dynamic>/
)