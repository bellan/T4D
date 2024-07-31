#include "Simulation.hpp"

#include <TFile.h>
#include <TLorentzVector.h>
#include <TMatrixD.h>
#include <TMatrixDfwd.h>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <vector>

#include "Detector.hpp"
#include "MeasuresAndStates.hpp"
#include "Particle.hpp"
#include "ParticleGun.hpp"
#include "PhisicalParameters.hpp"
#include "SetupFactory.hpp"
#include "Tracker.hpp"
#include "Utils.hpp"

/**
 * The default constructor TODO sobstitude with a factory that creates the
 * experiment settings
 */
Simulation::Simulation() : detectors(), dataFile() {
  SetupFactory factory{};
  const SimulationSetup experiment = factory.generateExperiment();
  detectors = experiment.detectors;
  tracker = Tracker(experiment.detectors);
  particleGun.setMaxColatitude(experiment.particleGun.getMaxColatitude());
  particleGun.setPosition(experiment.particleGun.getPosition());

  if (detectors.size() == 0) {
    throw std::invalid_argument("No detector");
  }

  minTimeInterval = MIN_TIME_BETWEEN_PARTICLE;
}

/**
 * The main simulation function
 *
 * TODO: determine what actually does
 */
void Simulation::runSimulation(int particlesNumber) {
  std::vector<Particle> evolvedParticles =
      generateParticlesAndEvolve(particlesNumber);
  std::vector<std::vector<ParticleState>> generatedParticlesStates;
  std::vector<Measurement> generatedMeasures;
  for (Particle particle : evolvedParticles) {
    std::vector<ParticleState> registeredParticleStates;
    std::vector<ParticleState> generatedParticleStates = particle.getStates();
    registeredParticleStates.push_back(generatedParticleStates[0]);
    for (int i = 0; i < (int)detectors.size(); i++) {
      std::optional<Measurement> measure =
          detectors[i].measure(generatedParticleStates[i + 1]);
      if (measure) {
        generatedMeasures.push_back(measure.value());
        registeredParticleStates.push_back(generatedParticleStates[i + 1]);
      }
    }
    generatedParticlesStates.push_back(registeredParticleStates);
  }
  dataFile.SaveMultipleMeasures(generatedMeasures);

  auto misure = dataFile.readMeasures();
  auto misureParticelle = separateMeasuresInParticles(misure);
  for (auto particella : misureParticelle) {
    std::cout << "PARTICELLA" << std::endl;
    for (auto misura : particella)
      std::cout << "Id: " << misura.detectorID << "      Misura: " << misura.t
                << " " << misura.x << " " << misura.y << std::endl;
  }

  std::vector<std::vector<MatrixStateEstimate>> predictedStates;
  std::vector<std::vector<MatrixStateEstimate>> filteredStates;
  std::vector<std::vector<MatrixStateEstimate>> smoothedStates;
  for (int j = 0; j < (int)generatedParticlesStates.size(); j++) {
    auto misureParticellaSingola = misureParticelle[j];
    auto statiParticellaSingola = generatedParticlesStates[j];
    std::vector<MatrixStateEstimate> predizioni;
    std::vector<MatrixStateEstimate> statiFiltrati =
        tracker.kalmanFilter(misureParticellaSingola, predizioni);
    std::vector<MatrixStateEstimate> statiSmoothed =
        tracker.kalmanSmoother(statiFiltrati);
    predictedStates.push_back(predizioni);
    filteredStates.push_back(statiFiltrati);
    smoothedStates.push_back(statiSmoothed);
  }

  saveDataToCSV(detectors, generatedParticlesStates, filteredStates,
                smoothedStates, predictedStates);
}

/**
 * Generate a set of particles and the corresponding measurements
 *
 * @param particleNumber the number of particles to be generated
 * @return a vector containing the measurements generated
 */
// TODO: Remove printing
std::vector<Measurement> Simulation::generateMeasures(int particlesNumber) {
  std::vector<Measurement> measureVector;
  for (int i = 0; i < particlesNumber; i++) {
    Particle particle = particleGun.generateParticle();
    std::cout << " - " << particle.getStates()[0].position.T() << " "
              << particle.getStates()[0].position.X() << " "
              << particle.getStates()[0].position.Y() << " "
              << particle.getStates()[0].position.Z() << " " << " - "
              << particle.getStates()[0].velocity.Mag() << " "
              << particle.getStates()[0].velocity.X() << " "
              << particle.getStates()[0].velocity.Y() << " "
              << particle.getStates()[0].velocity.Z() << " " << std::endl;

    for (Detector detector : detectors) {
      auto position =
          particle.zSpaceEvolve(detector.getBottmLeftPosition().z());

      std::optional<Measurement> measure = detector.measure(position);
      if (measure)
        measureVector.push_back(measure.value());
    }
  }

  return measureVector;
}

/**
 * Generate a set of particles and make them evolve
 *
 * @param particleNumber the number of particles to be generated
 * @return a vector containing the particles
 */
std::vector<Particle>
Simulation::generateParticlesAndEvolve(int particlesNumber) {
  std::vector<Particle> particlesVector;
  for (int i = 0; i < particlesNumber; i++) {
    Particle particle = particleGun.generateParticle();
    for (Detector detector : detectors) {
      particle.zSpaceEvolve(detector.getBottmLeftPosition().z());
    }
    particlesVector.push_back(particle);
  }

  return particlesVector;
}

/**
 * Divide the measurement Vector into a vector of vectors containing measures
 * from a single particle for each vector.
 *
 * It first orders the measures based on the time they occour. Then devides them
 * based on the detector id: if the detector of a measure is lower than that of
 * the preavious measure, this is registered as a new particle.
 *
 * NOTE: This algorithm assumes that before a particle is shot the preavious one
 * has already passed through all the detectors
 * TODO: Implement the abose (increasing the spees should be enough) and
 * uncomment the line in the code. Othewise change the description.
 *
 * @param allMeasures the vector containing the complete sequence of
 * measurements.
 * @param (out)(optional) particlesNumber the number of the particles
 *
 * @return a vector of vector each one representing a single particle
 */
std::vector<std::vector<Measurement>>
Simulation::separateMeasuresInParticles(std::vector<Measurement> allMeasures) {
  if (allMeasures.empty())
    throw std::invalid_argument("No measures given");

  // std::sort(allMeasures.begin(), allMeasures.end(), [](Measurement a,
  // Measurement b){return a.t < b.t;});

  std::vector<std::vector<Measurement>> singleParticleMeasuresVectors;

  for (Measurement measure : allMeasures) {
    if (singleParticleMeasuresVectors.empty()) {
      singleParticleMeasuresVectors.push_back(std::vector<Measurement>());
    } else if (measure.detectorID <
               singleParticleMeasuresVectors.back().back().detectorID) {
      singleParticleMeasuresVectors.push_back(std::vector<Measurement>());
    }
    singleParticleMeasuresVectors.back().push_back(measure);
  }

  return singleParticleMeasuresVectors;
}
