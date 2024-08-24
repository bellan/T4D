#pragma once

#include <TLorentzVector.h>
#include <TMatrixD.h>
#include <TMatrixDfwd.h>
#include <vector>

#include "DataFile.hpp"
#include "DataGenerator.hpp"
#include "Detector.hpp"
#include "Tracker.hpp"

class Simulation {
public:
  Simulation();
  void runSimulation(int particlesNumber);

private:
  std::vector<Detector> detectors;

  DataFile dataFile;
  Tracker tracker;
  DataGenerator dataGenerator;
};
