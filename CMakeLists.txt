cmake_minimum_required(VERSION 3.26 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 20)
project(Tracker)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# source directories
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(Tracker main.cpp ${sources})
