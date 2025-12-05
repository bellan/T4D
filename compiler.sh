#!/bin/bash
set -e

# ~~~ End message
end_message() {
    echo " ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    echo " The execution of the script has ended successfully. Please read the"
    echo " previous printouts to get information about the status of the simulation."
    echo " Thank you for your patience!"
    echo " ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
}

# ~~~ Compiling the code
compile() {
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

    echo ""
}

# ~~~ Running the code
run() {
    # --- Executing the code
    echo " ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    echo " --- Executing the code"
   ./Tracking_simulation

   echo ""
}

# ~~~ Executing the macros for the visualisation
show(){
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
    echo " - Detector_test.py script (2/2)"
    if find ../results -type f -name "*Detector test*" | grep -q .; then
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

    echo " - DetectorTesting.py script (4/4)"
    # Only when Detector test files are present:
    if find ../results -type f -name "*Detector test*" | grep -q .; then
        python DetectorTesting.py
    else
        echo " Results for Detector testing not found."
    fi

    # -- Deactivation of the virtual environment
    deactivate

    echo ""
}

# ~~~ Compiling and running the code
compile_run() {
    compile
    run
    end_message
}

# ~~~ Compiling and running the code and showing the results
compile_run_show() {
    compile
    run
    show
    end_message
}

$1