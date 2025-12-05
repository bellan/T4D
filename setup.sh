#!/bin/bash
set -e

# ~~~ Global variables
# Path to python virtual environment
DIR_venv="env"

# Number of thread
NUM_threads=$(($(nproc) -1))
if [ "$NUM_threads" -lt 1 ]; then
    NUM_threads=1
fi

# Variables for the update_requirements function
DIR_project="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
FILE_requirements="$DIR_project/ProjectDesign/requirements.txt"

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
    mkdir -p "./raw"
    mkdir -p "./samples"

    # --- Creating a virtual environment for Python
    echo " --- Creating virtual environment"
    if [ ! -d "$DIR_venv" ]; then
        python3 -m venv "$DIR_venv"
    fi

    # --- Activating the virtual environment
    echo " --- Activating virtual environment"
    source $DIR_venv/bin/activate

    # --- Installing required Python packages
    echo " --- Installing required Python packages"
    if [ -f "$FILE_requirements" ]; then
        pip install -r $FILE_requirements
    else
        echo " $FILE_requirements not found"
    fi

    # --- Deactivating the virtual environment
    echo " --- Deactivating virtual environment"
    deactivate

    echo ""
    echo " ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    echo " Setup completed successfully."
    echo " Thank you for your patience!"
}

# ~~~ Command to update Python requirements
update_requirements() {
    echo " ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    echo " ~~~ Updating Python requirements file"
    echo ""

    # --- Activating the virtual environment
    echo " --- Activating virtual environment"
    source $DIR_venv/bin/activate
    echo ""

    # --- Verifying if pip is up to date
    echo " --- Checking requirements"
    echo " Upgrading pip to the latest version"
    pip install --upgrade pip
    echo ""

    # --- Verifying if pipreq is installed
    echo " Verifying if pipreqs is installed (if not, installing it)"
    if ! command -v pipreqs &> /dev/null
    then
        pip install pipreqs
    fi
    echo ""

    # --- Updating required Python packages
    echo " --- Updating requirements.txt file"
    pipreqs "$DIR_project" \
        --force \
        --encoding=utf-8 \
        --savepath "$FILE_requirements"
    echo ""

    # --- Deactivating the virtual environment
    echo " --- Deactivating virtual environment"
    deactivate

    echo ""
    echo " ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
    echo " Python packages required list updated successfully."
    echo " Thank you for your patience!"
}

$1