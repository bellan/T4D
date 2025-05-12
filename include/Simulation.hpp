#pragma once

// Header files needed
#include <TFile.h>
#include <TLorentzVector.h>
#include <TTree.h>
#include <vector>

// Custom classes
#include "Detector.hpp"
#include "Measure.hpp"
#include "ParticleGun.hpp"
#include "ParticleState.hpp"
#include "StructMeasures.hpp"

// Namespaces
using namespace std;



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Data - struct
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Struct to save the particle states of the generated particles for each layer
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
};
