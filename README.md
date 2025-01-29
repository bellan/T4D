# 4D-tracking-CMS
Tracking system for the new generation of silicon sensors used in the CMS experiment at CERN. 
More information about the framework can be found in the ```ProjectDesign``` folder.


## Setup
To start using this framework, ROOT, CMake and Python must be installed.
Then, to correctly set up your working area, clone the repository and run the following commands in the main folder:

```console
git clone https://github.com/bellan/T4D.git
cd T4D
bash compiler.sh setup
```

This will create all the necessary directories and set up a Python virtual environment with the required libraries to run the analysis and visualization scripts.


## Execution
To compile and execute the simulation, run the following command in the main folder:

```console
bash compiler.sh compile_run
```

To compile, execute the simulation, and run the visualization scripts, use this command:

```console
bash compiler.sh compile_run_show
```

