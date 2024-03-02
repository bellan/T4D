#!/bin/bash

compile_run() {
    cd build
    echo "Creating make file"
    cmake ..
    echo "Compiling"
    make -j 4
    echo "Executing"
    ./Tracker
}

$1
