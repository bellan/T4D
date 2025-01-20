##################################
## A. Orione (UNITO) - Feb 2024 ##
## E. Racca (UNITO)  - Gen 2025 ##
##################################

# --- Minimum requirements
cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

# --- Project information
project(Tracking_simulation)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_CXX_STANDARD 20)

# --- Locate the ROOT package
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT 6.30 REQUIRED COMPONENTS)

# --- Flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} `root-config --cflags --ldflags`")

# --- Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)
include(${ROOT_USE_FILE})

# --- Source directories
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cpp)

# --- Compiler options
add_compile_options(-Wall -Wextra -Wpedantic)

# --- Executables and targets
add_executable(Tracking_simulation main.cpp ${sources})
target_link_libraries(Tracking_simulation PUBLIC ${ROOT_LIBRARIES})
target_include_directories(Tracking_simulation PUBLIC ${ROOT_INCLUDE_DIRS})