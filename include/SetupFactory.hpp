#pragma once

#include "Detector.hpp"
#include "ParticleGun.hpp"

#include <vector>

struct SimulationSetup {
public:
  ParticleGun particleGun;
  std::vector<Detector> detectors;
};

class SetupFactory {
public:
  SetupFactory() {}

  /**
   * Generate the simulation setup according to values specified in
   * Phisical_parameters.hpp
   *
   * @return the simulation setup generated
   */
  SimulationSetup generateExperiment() const;
};
