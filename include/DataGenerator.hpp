#pragma once

#include "MeasuresAndStates.hpp"
#include "Particle.hpp"
#include "SetupFactory.hpp"

#include <vector>

struct GeneratedData {
  std::vector<std::vector<ParticleState>> allParticlesTheoreticalStates;
  std::vector<std::vector<ParticleState>> allParticlesRealStates;
  std::vector<std::vector<Measurement>> allParticlesMeasures;
};

class DataGenerator {
public:
  DataGenerator(){};
  DataGenerator(SimulationSetup simulationSetup)
      : simulationSetup(simulationSetup){};

  Particle generateParticle() {
    return simulationSetup.particleGun.generateParticle();
  };

  /**
   * Generate the states of a given particle.
   *
   * @param particle the particle to be evolved.
   * @param multipleScattering whether to use multiple scattering.
   * @return a vector containing the particles.
   */
  std::vector<ParticleState>
  generateParticleStates(Particle particle,
                         bool multipleScattering = true) const;
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
  std::vector<Measurement>
  generateParticleMeasures(std::vector<ParticleState> &ParticleStates) const;

  /**
   * Generate all the data for a given number of particles
   *
   * @param numberOfParticles the number of particles to be generated
   * @param logging whether or not to show log messages
   * @param useMultipleScattering whether or not to use multiple scattering during the evolution
   * @return a vector containing all the data of all the particles
   */
  GeneratedData generateAllData(int numberOfParticles, bool logging = false,
                                bool useMultipleScattering = true);

private:
  SimulationSetup simulationSetup;

  void logData(const GeneratedData &generatedData) const;
};
