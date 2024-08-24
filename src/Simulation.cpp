#include "Simulation.hpp"

#include <TFile.h>
#include <TLorentzVector.h>
#include <TMatrixD.h>
#include <TMatrixDfwd.h>
#include <stdexcept>
#include <vector>

#include "DataGenerator.hpp"
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
  GeneratedData generatedData =
      dataGenerator.generateAllData(particlesNumber, false, true);
  std::vector<Measurement> allMeasures =
      Utils::concatenateMeasures(generatedData.allParticlesMeasures);

  // DATA SAVING
  dataFile.SaveMultipleMeasures(allMeasures);

  // DATA ELABORATION
  allMeasures = dataFile.readMeasures();
  std::vector<std::vector<Measurement>> allParticlesMeasures =
      Utils::separateMeasuresInParticles(allMeasures);

  std::vector<std::vector<MatrixStateEstimate>> allParticlesPredictedStates;
  std::vector<std::vector<MatrixStateEstimate>> allParticlesFilteredStates;
  std::vector<std::vector<MatrixStateEstimate>> allParticlesSmoothedStates;
  for (std::vector<Measurement> particleMeasures : allParticlesMeasures) {
    kalmanFilterResult filterResults = tracker.kalmanFilter(particleMeasures, false);
    std::vector<MatrixStateEstimate> predictedStates =
        filterResults.predictedStates;
    std::vector<MatrixStateEstimate> filteredStates =
        filterResults.filteredStates;

    std::vector<MatrixStateEstimate> smoothedStates =
        tracker.kalmanSmoother(filteredStates, false);
    allParticlesPredictedStates.push_back(predictedStates);
    allParticlesFilteredStates.push_back(filteredStates);
    allParticlesSmoothedStates.push_back(smoothedStates);
  }

  Utils::saveDataToCSV(detectors, generatedData.allParticlesTheoreticalStates,
                       generatedData.allParticlesRealStates,
                       allParticlesMeasures, allParticlesPredictedStates,
                       allParticlesFilteredStates, allParticlesSmoothedStates);
}
