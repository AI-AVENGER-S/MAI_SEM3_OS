# Требуемая минимальная версия CMake
cmake_minimum_required(VERSION 3.10)

# Название проекта
project(KMeansLab CXX)

# Устанавливаем стандарт C++ (C++17 или C++14/C++11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Находим библиотеку, необходимую для std::thread (на Unix это pthreads)
# CMake сам найдет нужную библиотеку и для Windows
find_package(Threads REQUIRED)

# Говорим CMake, что заголовочные файлы лежат в папке 'include'
include_directories(include)

# Создаем исполняемый файл 'kmeans_lab' из двух исходников
add_executable(kmeans_lab main.cpp src/solution.cpp)

# Прилинковываем (подключаем) библиотеку потоков к нашему файлу
# Threads::Threads - это современный способ сделать это в CMake
target_link_libraries(kmeans_lab PRIVATE Threads::Threads)