#include "SetupFactory.hpp"

#include "Detector.hpp"
#include "ParticleGun.hpp"
#include "PhisicalParameters.hpp"

#include <vector>

SimulationSetup SetupFactory::generateExperiment() const {
  std::vector<Detector> detectors;
  detectors.reserve(NUMBER_OF_DETECTORS);
  for (int i = 1; i < NUMBER_OF_DETECTORS + 1; i++) {
    detectors.push_back(Detector(i * DISTANCE_BETWEEN_DETECTORS,
                                 DETECTOR_DIMENSION, DETECTOR_DIMENSION));
  }

  const ParticleGun gun({0, 0, 0}, detectors);
  return SimulationSetup{gun, detectors};
}
