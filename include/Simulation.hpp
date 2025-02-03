#pragma once

#include <TLorentzVector.h>
#include <TMatrixD.h>
#include <TMatrixDfwd.h>
#include <vector>

#include "DataGenerator.hpp"
#include "Detector.hpp"
#include "Tracker.hpp"

class Simulation {
public:
  Simulation();

  /**
   * The main simulation function.
   *
   * @param particlesNumber the number of particles to be simulated.
   */
  void runSimulation(int particlesNumber);

  /**
   * The main simulation function.
   *
   * @param particlesNumber the number of particles to be simulated.
   */
  void testDetector(int particlesNumber, int detectorId);

private:
  static int runCounter;
  std::vector<Detector> detectors;

  Tracker tracker;
  DataGenerator dataGenerator;
};
