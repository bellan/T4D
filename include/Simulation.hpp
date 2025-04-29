#pragma once

#include <TLorentzVector.h>
#include <TMatrixD.h>
#include <TMatrixDfwd.h>
#include <TFile.h>
#include <TTree.h>
#include <vector>

#include "DataGenerator.hpp"
#include "Detector.hpp"
#include "Structs.hpp"
#include "Tracker.hpp"


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Data - struct
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Struct to save the data
struct Data{
  // Particle gun
  ParticleGun gun;

  // Hits on each detector
  std::vector<ParticleState> lay0_particles;
  std::vector<ParticleState> lay1_particles;
  std::vector<ParticleState> lay2_particles;
  std::vector<ParticleState> lay3_particles;
  std::vector<ParticleState> lay4_particles;
  std::vector<ParticleState> lay5_particles;
  std::vector<ParticleState> lay6_particles;
  std::vector<ParticleState> lay7_particles;
  std::vector<ParticleState> lay8_particles;
};

struct Measures{
  // Hits on each detector
  std::vector<Measure> lay1_particles;
  std::vector<Measure> lay2_particles;
  std::vector<Measure> lay3_particles;
  std::vector<Measure> lay4_particles;
  std::vector<Measure> lay5_particles;
  std::vector<Measure> lay6_particles;
  std::vector<Measure> lay7_particles;
  std::vector<Measure> lay8_particles;
};



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Simulation - class
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class Simulation {
public:
  // --- Constructors
  Simulation ();
  Simulation(std::vector<Detector> detectors);

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

  TTree tree_generated;
  TTree tree_detector;
  TTree tree_measures;

  Data data_generated;
  Data data_detector;
  Measures data_measures;

  std::vector<Detector> detectors;

  static int runCounter; // TO BE REMOVED after the separation of the code
  Tracker tracker; // TO BE REMOVED after the separation of the code
  DataGenerator dataGenerator; // TO BE REMOVED after the separation of the code

};
