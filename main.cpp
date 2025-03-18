// Standard libraries
#include <chrono>
#include <ctime>
#include <TStopwatch.h>

// Custom classes and definitions
#include "PhysicalParameters.hpp"
#include "Detector.hpp"
#include "Simulation.hpp"
#include "ParticleGun.hpp"
#include "ParticleState.hpp"

// Namespaces
using namespace std;


int main() {
  // --- Starting time
  cout << "\n --------------------------------------------------------------------------" << endl;
  auto time_now = chrono::system_clock::to_time_t(chrono::system_clock::now());
  cout << " Execution begins at: " << ctime(&time_now) << endl;

  
  // --- Timing variables definition
  TStopwatch time_execution;
  TStopwatch time_generation;
  TStopwatch time_reconstruction;
  TStopwatch time_tracking;

  // Starting execution timer
  time_execution.Start();


  // --- Experiment
  cout << " --- Creating experiment" << endl;
  // Detectors
  vector<Detector> detectors_ele;
  detectors_ele.reserve(NUMBER_OF_DETECTORS);

  for (int i = 1; i < NUMBER_OF_DETECTORS + 1; i++) {
    detectors_ele.push_back(Detector(i * DISTANCE_BETWEEN_DETECTORS, DETECTOR_DIMENSION_WIDTH, DETECTOR_DIMENSION_HEIGHT, i));
  }

  // Creation of the simulation & output file
  Simulation simu(detectors_ele);

 
  // --- Generation
  cout << " --- Begining generation" << endl;
  time_generation.Start();
  time_now = chrono::system_clock::to_time_t(chrono::system_clock::now());
  bool bool_gen = simu.Generation();
  //simulation.runSimulation(NUMBER_OF_PARTICLES);

  cout << "Generation: " << bool_gen << endl;

  time_generation.Stop();

  /*
  // --- Simulation of detector response
  cout << "\n --- Begining reconstruction" << endl;
  time_reconstruction.Start();
  auto reconstruction = Reconstruction();
  reconstruction.runReconstruction(NUMBER_OF_PARTICLES);

  time_reconstruction.Stop();


  // --- Track reconstruction
  cout << "\n --- Beginning tracking" << endl;
  time_tracking.Start();
  auto tracker = Filter();
  tracker.runFilter(NUMBER_OF_PARTICLES);

  time_tracking.Stop();
  */


  // --- Simulation of layers testing
  //cout << "\n --- Begining layer testing" << endl;
  //simulation.testDetector(NUMBER_OF_PARTICLES, 5);
  //simulation.testDetector(1, 5);


  // --- Execution time
  cout << "\n --------------------------------------------------------------------------" << endl;
  time_now = chrono::system_clock::to_time_t(chrono::system_clock::now());
  time_execution.Stop();

  cout << " Execution ended at:   " << ctime(&time_now) << endl;
  cout << " Total execution time: "; time_execution.Print();
  cout << " Generation time:      "; time_generation.Print();
  cout << " Reconstruction time:  "; time_reconstruction.Print();
  cout << " Tracking time:        "; time_tracking.Print();
  cout << " --------------------------------------------------------------------------" << endl;

  return 0;
}