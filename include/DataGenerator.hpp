#pragma once

#include "MeasuresAndStates.hpp"
#include "Particle.hpp"
#include "SetupFactory.hpp"

#include <vector>

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

private:
  SimulationSetup simulationSetup;
};
