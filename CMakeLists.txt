cmake_minimum_required(VERSION 3.26 FATAL_ERROR)
# set(CMAKE_CXX_STANDARD 20)
project(Tracking_simulation)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(ROOT 6.30 REQUIRED COMPONENTS)

# include directories
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} `root-config --cflags --ldflags`")

include_directories(${PROJECT_SOURCE_DIR}/include)

include(${ROOT_USE_FILE})

# source directories
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_compile_options(-Wall -Wextra -Wpedantic)
#
add_executable(Tracking_simulation main.cpp ${sources})
target_link_libraries(Tracker PUBLIC ${ROOT_LIBRARIES})
target_include_directories(Tracker PUBLIC ${ROOT_INCLUDE_DIRS})
