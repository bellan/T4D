#include "DataGenerator.hpp"

#include "MeasuresAndStates.hpp"

#include <iomanip>
#include <vector>

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

std::vector<Measurement> DataGenerator::generateParticleMeasures(
    std::vector<ParticleState> &particleStates) const {
  std::vector<Measurement> measureVector;
  for (ParticleState state : particleStates) {
    if (!state.detectorID)
      continue;

    std::optional<Measurement> measure =
        simulationSetup.detectors[state.detectorID.value()].measure(state);
    if (!measure) {
      const int id = state.detectorID.value();
      std::cout << "Particle went out of detector line at detector " << id + 1
                << " (id = " << id << ")" << std::endl;
      break;
    }
    measureVector.push_back(measure.value());
  }

  return measureVector;
}

GeneratedData DataGenerator::generateAllData(int particlesNumber,
                                             bool enableLogging,
                                             bool useMultipleScattering) {
  std::vector<std::vector<ParticleState>> allParticlesTheoreticalStates;
  std::vector<std::vector<ParticleState>> allParticlesRealStates;
  std::vector<std::vector<Measurement>> allParticlesMeasures;

  for (int i = 0; i < particlesNumber; i++) {
    Particle particle = generateParticle();
    std::vector<ParticleState> theoreticalParticleStates =
        generateParticleStates(particle, false);
    std::vector<ParticleState> realParticleStates =
        generateParticleStates(particle, useMultipleScattering);
    std::vector<Measurement> particleMeasures =
        generateParticleMeasures(realParticleStates);

    allParticlesTheoreticalStates.push_back(theoreticalParticleStates);
    allParticlesRealStates.push_back(realParticleStates);
    allParticlesMeasures.push_back(particleMeasures);
  }

  const GeneratedData results{allParticlesTheoreticalStates,
                              allParticlesRealStates, allParticlesMeasures};
  if (enableLogging)
    logData(results);

  return results;
}

void DataGenerator::logData(const GeneratedData &generatedData) const {
  std::cout << "GENERATED DATA LOGS" << std::endl;
  std::cout << std::setprecision(5);
  const std::vector<std::vector<ParticleState>> &allParticlesTheoreticalStates =
      generatedData.allParticlesTheoreticalStates;
  const std::vector<std::vector<ParticleState>> &allParticlesRealStates =
      generatedData.allParticlesRealStates;
  const std::vector<std::vector<Measurement>> &allParticlesMeasures =
      generatedData.allParticlesMeasures;
  for (int j = 0; j < (int)allParticlesMeasures.size(); j++) {
    std::cout << "Particle " << j << "\n";
    std::cout << "z    |theoretical        |real     |measured\n";
    std::cout << "z    |t,x,y,1/vz,xz,yz|t,x,y,1/vz,xz,yz|t,x,y,t,x,y\n";

    const std::vector<Measurement> &measures = allParticlesMeasures[j];
    const std::vector<ParticleState> &theoStates =
        allParticlesTheoreticalStates[j];
    const std::vector<ParticleState> &realStates = allParticlesRealStates[j];
    std::cout << theoStates[0].position.Z() << " | ";
    std::cout << theoStates[0].position.T() << "," << theoStates[0].position.X()
              << "," << theoStates[0].position.Y() << ","
              << 1. / theoStates[0].velocity.Z() << ","
              << theoStates[0].velocity.X() / theoStates[0].velocity.Z() << ","
              << theoStates[0].velocity.Y() / theoStates[0].velocity.Z() << "|";

    std::cout << realStates[0].position.T() << "," << realStates[0].position.X()
              << "," << realStates[0].position.Y() << ","
              << 1. / realStates[0].velocity.Z() << ","
              << realStates[0].velocity.X() / realStates[0].velocity.Z() << ","
              << realStates[0].velocity.Y() / realStates[0].velocity.Z()
              << std::endl;
    for (int i = 0; i < (int)measures.size(); i++) {
      Measurement meas = measures[i];
      ParticleState the = theoStates[i + 1];
      ParticleState rea = realStates[i + 1];
      std::cout << the.position.Z() << " | ";
      std::cout << the.position.T() << "," << the.position.X() << ","
                << the.position.Y() << "," << 1. / the.velocity.Z() << ","
                << the.velocity.X() / the.velocity.Z() << ","
                << the.velocity.Y() / the.velocity.Z() << "|";

      std::cout << rea.position.T() << "," << rea.position.X() << ","
                << rea.position.Y() << "," << 1. / rea.velocity.Z() << ","
                << rea.velocity.X() / rea.velocity.Z() << ","
                << rea.velocity.Y() / rea.velocity.Z() << "|";

      std::cout << meas.t << "," << meas.x << "," << meas.y << std::endl;
    }
  }
  std::cout << "\n" << std::endl;
}
