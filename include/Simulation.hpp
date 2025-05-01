#pragma once

// Header files needed
#include <TLorentzVector.h>
#include <TMatrixD.h> // TO BE REMOVED after code separation
#include <TMatrixDfwd.h> // TO BE REMOVED after code separation
#include <TFile.h>
#include <TTree.h>
#include <vector>

// Custom classes
#include "DataGenerator.hpp" // TO BE REMOVED after code separation
#include "Detector.hpp"
#include "Measure.hpp"
#include "Structs.hpp"
#include "Tracker.hpp" // TO BE REMOVED after code separation

// Namespaces
using namespace std;



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Data - struct
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Struct to save the particle states of the generated particles
struct Data{
  // Particle gun
  ParticleGun gun;

  // Hits on each detector
  vector<ParticleState> lay0_particles;
  vector<ParticleState> lay1_particles;
  vector<ParticleState> lay2_particles;
  vector<ParticleState> lay3_particles;
  vector<ParticleState> lay4_particles;
  vector<ParticleState> lay5_particles;
  vector<ParticleState> lay6_particles;
  vector<ParticleState> lay7_particles;
  vector<ParticleState> lay8_particles;
};



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Simulation - class
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class Simulation {
public:
  // --- Constructors
  Simulation ();
  Simulation(const vector<Detector>& detectors);

  // --- Destructors
  virtual ~Simulation ();

  // --- Member functions
  /**
   * The main simulation function. TO BE REMOVED after the separation of the code
   *
   * @param particlesNumber the number of particles to be simulated.
   */
  void runSimulation(int particlesNumber);

  /**
   * Generation
   * Function to simulate the generated particles.
   * 
   * @returns bool 1 when successful, 0 when errors occured
   */
  bool Generation();

  /**
   * DetectorResponse
   * Function to simulate the detector response to the particles.
   * 
   * @returns bool 1 when successful, 0 when errors occured
   */
  bool DetectorResponse();

  /**
   * Measurement
   * Function to simulate the measurement of the particles.
   * 
   * @returns bool 1 when successful, 0 when errors occured
   */
  bool Measurement();

  /**
   * testDetector
   * Simulates the testing of a specific detector.
   * TO BE REFORMED after the changes in the main simulation
   *
   * @param particlesNumber the number of particles to be simulated.
   */
  void testDetector(int particlesNumber, int detectorId);

private:
  // --- Data members
  TFile file_out;

  TTree* tree_generated;
  TTree* tree_detector;
  TTree* tree_measures;

  Data data_generated;
  Data data_detector;
  Measures data_measures;

  vector<Detector> detectors;

  static int runCounter; // TO BE REMOVED after the separation of the code
  Tracker tracker; // TO BE REMOVED after the separation of the code
  DataGenerator dataGenerator; // TO BE REMOVED after the separation of the code

};
