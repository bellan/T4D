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

  std::vector<ParticleState>
  generateParticleStates(Particle particle,
                         bool multipleScattering = true) const;
  std::vector<Measurement>
  generateParticleMeasures(std::vector<ParticleState> &ParticleStates) const;

  GeneratedData generateAllData(int numberOfParticles, bool logging = false,
                                bool useMultipleScattering = true);

private:
  SimulationSetup simulationSetup;

  void logData(const GeneratedData &generatedData) const;
};
