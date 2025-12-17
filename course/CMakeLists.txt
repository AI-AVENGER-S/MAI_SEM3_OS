cmake_minimum_required(VERSION 3.10)
project(DAGScheduler)

set(CMAKE_CXX_STANDARD 17)

if(APPLE)
    include_directories("/opt/homebrew/include")
    link_directories("/opt/homebrew/lib")
endif()
# 1. Ищем библиотеку в системе
find_package(yaml-cpp REQUIRED)

# 2. Указываем, где лежат наши .h файлы
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# 3. Собираем исполняемый файл из исходников
add_executable(dag_scheduler
    src/main.cpp
    src/DAG.cpp
    src/Executor.cpp
)

# 4. Линкуем библиотеку к нашему проекту
target_link_libraries(dag_scheduler yaml-cpp)