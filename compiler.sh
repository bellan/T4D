#!/bin/bash

# ~~~ Command to execute the first time
setup() {
    # --- Creating useful directories
    mkdir -p "./build"
    mkdir -p "./data"
    mkdir -p "./figures/StatisticalAnalysis"
    mkdir -p "./figures/TrackVisualization"
    mkdir -p "./results"
}

# ~~~ Compiling and running the code
compile_run() {
    # --- Compiling the code
    cd build
    echo "Creating make file"
    cmake ..
    echo "Compiling"

    # Adjusting the compilation to the numer of threads available
    #make -j 4
    num_threads=$(($(nproc) -1))
    if [ "$num_threads" -lt 1 ]; then
        num_threads=1
    fi
    
    make -j $num_threads

    # --- Executing the code
    echo "Executing"
    ./Tracking_simulation
}

# ~~~ Compiling and running the code and showing the results
compile_run_show() {
    # --- Compiling the code
    cd build
    echo "Creating make file"
    cmake ..
    echo "Compiling"

    # Adjusting the compilation to the numer of threads available
    num_threads=$(($(nproc) -1))
    if [ "$num_threads" -lt 1 ]; then
        num_threads=1
    fi
    make -j $num_threads

    # --- Executing the code
    echo "Executing"
    ./Tracking_simulation

    # --- Visualisation of data and results
    echo "

    "
    echo "Creating data visualisation"
    cd ../DataVisualization
    python dataVis.py
    cd ..
}

# ~~~ Visualization of data and results
show() {
    # --- Visualisation of data and results
    echo "Creating data visualisation"
    cd DataVisualization
    python dataVis.py
    cd ..
}

$1