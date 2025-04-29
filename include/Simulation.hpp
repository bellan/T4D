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

class Simulation {
public:
  Simulation ();
  Simulation(std::vector<Detector> detectors);

  virtual ~Simulation ();

  /**
   * The main simulation function.
   *
   * @param particlesNumber the number of particles to be simulated.
   */
  void runSimulation(int particlesNumber);

  /**
   * Function to simulate the generated particles.
   * 
   * @param detectors the experiments detector.
   */
  bool Generation();

  /**
   * Function to simulate the reconstruction of the particles.
   * 
   * @param detectors the experiments detector.
   */
  bool DetectorResponse();

  /**
   * The main simulation function.
   *
   * @param particlesNumber the number of particles to be simulated.
   */
  void testDetector(int particlesNumber, int detectorId);

private:
  TFile file_out;
  TTree tree_generated;
  TTree tree_detector;
  Data data_generated;
  Data data_detector;

  static int runCounter;
  std::vector<Detector> detectors;

  Tracker tracker;
  DataGenerator dataGenerator;

};
