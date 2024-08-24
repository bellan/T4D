#include "DataGenerator.hpp"
#include "MeasuresAndStates.hpp"
#include <vector>

/**
 * Generate the states of a given particle.
 *
 * @param particle the particle to be evolved.
 * @param multipleScattering whether to use multiple scattering.
 * @return a vector containing the particles.
 */
std::vector<ParticleState>
DataGenerator::generateParticleStates(Particle particle,
                                      bool multipleScattering) const {
  std::vector<ParticleState> particleStates;
  particleStates.push_back(particle.getInitialState());

  for (Detector detector : simulationSetup.detectors) {
    const ParticleState newState = particle.zSpaceEvolve(
        particleStates.back(), detector.getBottmLeftPosition().z(),
        multipleScattering, detector.getId());
    particleStates.push_back(newState);
  }
  return particleStates;
}

/**
 * Generate the measures from the states.
 *
 * This function loops through all the states.
 * If a state is associated to a detector than it generates the corresponding
 * measure.
 *
 * @param particleStates the states to be measured
 * @return a vector containing the measurements generated
 */
std::vector<Measurement> DataGenerator::generateParticleMeasures(
    std::vector<ParticleState> &particleStates) const {
  std::vector<Measurement> measureVector;
  for (ParticleState state : particleStates) {
    if (!state.detectorID)
      continue;

    std::optional<Measurement> measure =
        simulationSetup.detectors[state.detectorID.value()].measure(state);
    if (measure)
      measureVector.push_back(measure.value());
  }

  return measureVector;
}
