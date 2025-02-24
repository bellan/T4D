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
    echo " ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    echo " ~~~ Beginning the setup of the framework environment"
    echo ""

    # --- Creating useful directories
    echo " --- Creating useful directories"
    mkdir -p "./build"
    mkdir -p "./data"
    mkdir -p "./figures/StatisticalAnalysis"
    mkdir -p "./figures/TrackVisualization"
    mkdir -p "./results"

    # --- Creating a virtual environment for Python
    echo " --- Creating virtual environment"
    if [ ! -d "$VENV_DIR" ]; then
        python3 -m venv "$VENV_DIR"
    fi

    # --- Activating the virtual environment
    echo " --- Activating virtual environment"
    source $VENV_DIR/bin/activate

    # --- Installing required Python packages
    echo " --- Installing required Python packages"
    if [ -f "ProjectDesign/requirements.txt" ]; then
        pip install -r ProjectDesign/requirements.txt
    else
        echo " ProjectDesign/requirements.txt not found"
    fi

    # --- Deactivating the virtual environment
    echo " --- Deactivating virtual environment"
    deactivate

    echo ""
    echo " ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    echo " Setup completed successfully."
    echo " Thank you for your patience!"
}

# ~~~ Compiling and running the code
compile_run() {
    # --- Compiling the code
    echo " ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    echo " --- Compiling the code"
    cd build
    echo " Creating make files"
    cmake ..
    echo ""
    echo " Compiling"

    # Adjusting the compilation to the number of threads available    
    make -j $num_threads

    # --- Executing the code
    echo ""
    echo " ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    echo " --- Executing"
    ./Tracking_simulation

    echo ""
    echo " ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    echo " Compilation and execution completed successfully."
    echo " Thank you for your patience!"
}

# ~~~ Compiling and running the code and showing the results
compile_run_show() {
    # --- Compiling the code
    echo " ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    echo " --- Compiling the code"
    cd build
    echo " Creating make files"
    cmake ..
    echo ""
    echo " Compiling"

    # Adjusting the compilation to the number of threads available
    make -j $num_threads

    # --- Executing the code
    echo ""
    echo " ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    echo " --- Executing the code"
    echo " Executing"
    ./Tracking_simulation

    # --- Visualisation of data and results
    echo " ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    echo " --- Plots for statistical analysis"

    # -- Activation of the virtual environment
    cd ..
    source $VENV_DIR/bin/activate

    # -- Scripts for graphs
    cd ./StatisticalAnalysis
    echo " - Difference.py script (1/2)"
    python Difference.py

    # Only when Detector test files are present:
    if find ../results -type f -name "*Detector test*" | grep -q .; then
        echo " - Detector_test.py script (2/2)"
        python Detector_test.py
    else
        echo " Results for Detector testing not found."
        echo " The script will proceed with plots for track visualization."
    fi

    echo ""
    echo " ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    echo " --- Plots for track visualization"
    cd ../TrackVisualization
    echo " - Render.py script (1/4)"
    python Render.py

    echo " - TimeRenderer.py script (2/4)"
    python TimeRenderer.py

    echo " - TrackVis.py script (3/4)"
    python TrackVis.py

    # Only when Detector test files are present:
    if find ../results -type f -name "*Detector test*" | grep -q .; then
        echo " - DetectorTesting.py script (4/4)"
        python DetectorTesting.py
    else
        echo " Results for Detector testing not found. The script will end."
    fi

    # -- Deactivation of the virtual environment
    deactivate

    echo ""
    echo " ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    echo " Compilation, execution and generation of figures completed successfully."
    echo " Thank you for your patience!"
}

# ~~~ Visualization of data and results
show() {
    # --- Visualisation of data and results
    # -- Activation of the virtual environment
    source $VENV_DIR/bin/activate

    # -- Scripts for graphs
    echo " ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    echo " --- Plots for statistical analysis"
    cd ./StatisticalAnalysis
    echo " - Difference.py script (1/2)"
    python Difference.py

    # Only when Detector test files are present:
    if find ../results -type f -name "*Detector test*" | grep -q .; then
        echo " - Detector_test.py script (2/2)"
        python Detector_test.py
    else
        echo " Results for Detector testing not found."
        echo " The script will proceed with plots for track visualization."
    fi

    echo ""
    echo " ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    echo " --- Plots for track visualization"
    cd ../TrackVisualization
    echo " - Render.py script (1/4)"
    python Render.py

    echo " - TimeRenderer.py script (2/4)"
    python TimeRenderer.py

    echo " - TrackVis.py script (3/4)"
    python TrackVis.py

    # Only when Detector test files are present:
    if find ../results -type f -name "*Detector test*" | grep -q .; then
        echo " - DetectorTesting.py script (4/4)"
        python DetectorTesting.py
    else
        echo " Results for Detector testing not found. The script will end."
    fi

    # -- Deactivation of the virtual environment
    deactivate

    echo ""
    echo " ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    echo " Generation of figures completed successfully."
    echo " Thank you for your patience!"
}

$1