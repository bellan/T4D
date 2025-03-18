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

# --- Integrating ROOT's functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} `root-config --cflags --ldflags`")
include(${ROOT_USE_FILE})

# --- Including Python libraries
#find_package(PythonLibs REQUIRED)
#include_directories(${PYTHON_INCLUDE_DIRS})

# --- Include directories
#message("Include dirs = " ${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/include)
include(${ROOT_USE_FILE})

# --- Source directories
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cpp)

# --- Dictionary
set(EventDF_HEADERS include/*.hpp)
set(EventDF_LINKDEF include/STDToolsLinkDef.h)
ROOT_GENERATE_DICTIONARY(G_T4D_Dictionary "${EventDF_HEADERS}" LINKDEF "${EventDF_LINKDEF}")

# --- Compiler options
add_compile_options(-Wall -Wextra -Wpedantic)

# --- Executables and targets
add_library(Tracking_simulation_dict SHARED G_T4D_Dictionary.cxx)

add_executable(Tracking_simulation main.cpp ${sources})
target_link_libraries(Tracking_simulation PUBLIC ${ROOT_LIBRARIES})
target_include_directories(Tracking_simulation PUBLIC ${ROOT_INCLUDE_DIRS})
target_link_libraries(Tracking_simulation PUBLIC Tracking_simulation_dict)