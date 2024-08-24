#include "Simulation.hpp"

#include <TFile.h>
#include <TLorentzVector.h>
#include <TMatrixD.h>
#include <TMatrixDfwd.h>
#include <stdexcept>
#include <vector>

#include "MeasuresAndStates.hpp"
#include "Particle.hpp"
#include "SetupFactory.hpp"
#include "Tracker.hpp"
#include "Utils.hpp"

/**
 * The default constructor TODO: sobstitude with a factory that creates the
 * experiment settings
 */
Simulation::Simulation() : detectors(), dataFile() {
  SetupFactory factory{};
  const SimulationSetup experiment = factory.generateExperiment();
  detectors = experiment.detectors;
  dataGenerator = DataGenerator(experiment);
  tracker = Tracker(experiment.detectors);

  if (detectors.size() == 0) {
    throw std::invalid_argument("No detector");
  }
}

/**
 * The main simulation function
 *
 * TODO: determine what actually does
 */
void Simulation::runSimulation(int particlesNumber) {
  // DATA CREATION
  std::vector<std::vector<ParticleState>> allParticlesTheoreticalStates;
  std::vector<std::vector<ParticleState>> allParticlesRealStates;
  std::vector<std::vector<Measurement>> allParticlesMeasures;
  std::vector<Measurement> allMeasures;

  for (int i = 0; i < particlesNumber; i++) {
    Particle particle = dataGenerator.generateParticle();
    std::vector<ParticleState> theoreticalParticleStates =
        dataGenerator.generateParticleStates(particle, false);
    std::vector<ParticleState> realParticleStates =
        dataGenerator.generateParticleStates(particle, true);
    std::vector<Measurement> particleMeasures =
        dataGenerator.generateParticleMeasures(realParticleStates);

    for (Measurement measure : particleMeasures) {
      allMeasures.push_back(measure);
    }

    allParticlesTheoreticalStates.push_back(theoreticalParticleStates);
    allParticlesRealStates.push_back(realParticleStates);
    allParticlesMeasures.push_back(particleMeasures);
  }
  dataFile.SaveMultipleMeasures(allMeasures);

  // DATA ELABORATION
  allMeasures = dataFile.readMeasures();
  allParticlesMeasures = separateMeasuresInParticles(allMeasures);

  std::vector<std::vector<MatrixStateEstimate>> allParticlesPredictedStates;
  std::vector<std::vector<MatrixStateEstimate>> allParticlesFilteredStates;
  std::vector<std::vector<MatrixStateEstimate>> allParticlesSmoothedStates;
  for (std::vector<Measurement> particleMeasures : allParticlesMeasures) {
    kalmanFilterResult filterResults = tracker.kalmanFilter(particleMeasures);
    std::vector<MatrixStateEstimate> predictedStates =
        filterResults.predictedStates;
    std::vector<MatrixStateEstimate> filteredStates =
        filterResults.filteredStates;

    std::vector<MatrixStateEstimate> smoothedStates =
        tracker.kalmanSmoother(filteredStates);
    allParticlesPredictedStates.push_back(predictedStates);
    allParticlesFilteredStates.push_back(filteredStates);
    allParticlesSmoothedStates.push_back(smoothedStates);
  }

  saveDataToCSV(detectors, allParticlesTheoreticalStates,
                allParticlesRealStates, allParticlesMeasures,
                allParticlesPredictedStates, allParticlesFilteredStates,
                allParticlesSmoothedStates);
}
