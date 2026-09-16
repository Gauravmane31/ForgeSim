cmake_minimum_required(VERSION 3.16)

project(ForgeSim
    VERSION 0.1.0
    DESCRIPTION "Incremental Parallel Engineering Simulation Engine"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(forgesim
    src/main.cpp
)

target_include_directories(forgesim
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
) 