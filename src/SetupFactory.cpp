// Header files needed
#include <vector>

// Custom classes
#include "SetupFactory.hpp"
#include "Detector.hpp"
#include "ParticleGun.hpp"
#include "PhysicalParameters.hpp"



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// generateExperiment
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SimulationSetup SetupFactory::generateExperiment() const {
  // Creation of the detectors acording to their geometry
  std::vector<Detector> detectors;
  detectors.reserve(NUMBER_OF_DETECTORS);

  for (int i = 1; i < NUMBER_OF_DETECTORS + 1; i++) {
    detectors.push_back(Detector(i * DISTANCE_BETWEEN_DETECTORS, DETECTOR_DIMENSION_WIDTH, DETECTOR_DIMENSION_HEIGHT));
  }

  // Creating the point of interaction
  const ParticleGun gun({0, 0, 0}, detectors);
  
  return SimulationSetup{gun, detectors};
}
