#!/bin/bash

setup() {
    if [ ! -d "./build" ]; then
        mkdir -p "./build"
    fi
    if [ ! -d "./data" ]; then
        mkdir -p "./data"
    fi
    if [ ! -d "./results" ]; then
        mkdir -p "./results"
    fi
}

compile_run() {
    cd build
    echo "Creating make file"
    cmake ..
    echo "Compiling"
    make -j 4
    echo "Executing"
    ./Tracking_simulation
}

compile_run_show() {
    cd build
    echo "Creating make file"
    cmake ..
    echo "Compiling"
    make -j 4
    echo "Executing"
    ./Tracking_simulation

    echo "

    "
    echo "Creating data visualization"
    cd ../DataVisualization
    python dataVis.py
    cd ..
}

show() {
    echo "Creating data visualization"
    cd DataVisualization
    python dataVis.py
    cd ..
}
$1
