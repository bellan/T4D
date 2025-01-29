#!/bin/bash

# ~~~ Global variables
# Path to python virtual environment
VENV_DIR="env"

# Number of thread
num_threads=$(($(nproc) -1))
if [ "$num_threads" -lt 1 ]; then
    num_threads=1
fi

# ~~~ Command to execute the first time
setup() {
    # --- Creating useful directories
    echo "Creating useful directories"
    mkdir -p "./build"
    mkdir -p "./data"
    mkdir -p "./figures/StatisticalAnalysis"
    mkdir -p "./figures/TrackVisualization"
    mkdir -p "./results"

    # --- Creating a virtual environment for Python
    echo "Creating virtual environment"
    if [ ! -d "$VENV_DIR" ]; then
        python3 -m venv "$VENV_DIR"
    fi

    # --- Activating the virtual environment
    echo "Activating virtual environment"
    source $VENV_DIR/bin/activate

    # --- Installing required Python packages
    echo "Installing required Python packages"
    if [ -f "ProjectDesign/requirements.txt" ]; then
        pip install -r ProjectDesign/requirements.txt
    else
        echo "ProjectDesign/requirements.txt not found"
    fi

    # --- Deactivating the virtual environment
    echo "Deactivating virtual environment"
    deactivate
}

# ~~~ Compiling and running the code
compile_run() {
    # --- Compiling the code
    cd build
    echo "Creating make file"
    cmake ..
    echo "Compiling"

    # Adjusting the compilation to the numer of threads available    
    make -j $num_threads

    # --- Executing the code
    echo "Executing"
    ./Tracking_simulation
}

# ~~~ Compiling and running the code and showing the results
compile_run_show() {
    # --- Compiling the code
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    echo " --- Compiling the code"
    cd build
    echo "Creating make file"
    cmake ..
    echo "Compiling"

    # Adjusting the compilation to the numer of threads available
    make -j $num_threads

    # --- Executing the code
    echo ""
    echo " --- Executing the code"
    echo "Executing"
    ./Tracking_simulation

    # --- Visualisation of data and results
    # Activation of the virtual environment
    cd ..
    source $VENV_DIR/bin/activate

    # Scripts for graphs
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    echo " --- Plots for statistical analysis"
    cd ./StatisticalAnalysis
    python Difference.py

    echo ""
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    echo " --- Plots for track visualization"
    cd ../TrackVisualization
    python Render.py
    python TimeRenderer.py
    python TrackVis.py

    # Deactivation of the virtual environment
    deactivate
}

# ~~~ Visualization of data and results
show() {
    # --- Visualisation of data and results
    # Activation of the virtual environment
    source $VENV_DIR/bin/activate

    # Scripts for graphs
    echo " --- Plots for statistical analysis"
    cd ../StatisticalAnalysis
    python Difference.py

    echo ""
    echo " --- Plots for track visualization"
    cd ../TrackVisualization
    python Render.py
    python TimeRender.py
    python TrackVis.py

    # Deactivation of the virtual environment
    deactivate
}

$1