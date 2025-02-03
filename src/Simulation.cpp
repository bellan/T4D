#include "Simulation.hpp"

#include "DataFile.hpp"
#include "DataGenerator.hpp"
#include "MeasuresAndStates.hpp"
#include "PhysicalParameters.hpp"
#include "SetupFactory.hpp"
#include "Tracker.hpp"
#include "Utils.hpp"

#include <TFile.h>
#include <TLorentzVector.h>
#include <TMatrixD.h>
#include <TMatrixDfwd.h>
#include <stdexcept>
#include <string>
#include <vector>

int Simulation::runCounter = 0;

Simulation::Simulation() : detectors() {
  SetupFactory factory{};
  const SimulationSetup experiment = factory.generateExperiment();
  detectors = experiment.detectors;
  dataGenerator = DataGenerator(experiment);
  tracker = Tracker(experiment.detectors);

  if (detectors.size() == 0) {
    throw std::invalid_argument("No detector");
  }
}

void Simulation::runSimulation(int particlesNumber) {
  // DATA CREATION
  GeneratedData generatedData =
      dataGenerator.generateAllData(particlesNumber, false, true);
  std::vector<Measurement> allMeasures =
      Utils::concatenateMeasures(generatedData.allParticlesMeasures);

  // DATA SAVING
  std::string dataFileName =
      "../data/GeneratedData_run" + std::to_string(runCounter) + ".root";
  DataFile dataFile = DataFile(dataFileName.c_str(), "DataTree", false);
  dataFile.SaveMultipleMeasures(allMeasures);

  // DATA ELABORATION
  allMeasures = dataFile.readMeasures();
  std::vector<std::vector<Measurement>> allParticlesMeasures =
      Utils::separateMeasuresInParticles(allMeasures);

  std::vector<std::vector<MatrixStateEstimate>> allParticlesPredictedStates;
  allParticlesPredictedStates.reserve(particlesNumber);
  std::vector<std::vector<MatrixStateEstimate>> allParticlesFilteredStates;
  allParticlesFilteredStates.reserve(particlesNumber);
  std::vector<std::vector<MatrixStateEstimate>> allParticlesSmoothedStates;
  allParticlesSmoothedStates.reserve(particlesNumber);
  for (int i = 0; i < (int)allParticlesMeasures.size(); i++) {
    kalmanFilterResult filterResults =
        tracker.kalmanFilter(allParticlesMeasures[i], false, false);
    std::vector<MatrixStateEstimate> predictedStates =
        filterResults.predictedStates;
    std::vector<MatrixStateEstimate> filteredStates =
        filterResults.filteredStates;

    std::vector<MatrixStateEstimate> smoothedStates =
        tracker.kalmanSmoother(filteredStates, false);
    allParticlesPredictedStates.push_back(predictedStates);
    allParticlesFilteredStates.push_back(filteredStates);
    allParticlesSmoothedStates.push_back(smoothedStates);

    /*std::cout << "Filtered Chi2" << std::endl;*/
    /*tracker.computeChi2s(generatedData.allParticlesRealStates[i],*/
    /*                     filteredStates, true, true);*/
    /*std::cout << "\nSmoothed Chi2" << std::endl;*/
    /*tracker.computeChi2s(generatedData.allParticlesRealStates[i],*/
    /*                     smoothedStates, true, true);*/

    /*std::string fileName("../results/Particle ");*/
    /*fileName += std::to_string(i);*/
    /*fileName += ".root";*/
    /**/
    /*ResultFile resultFile(fileName.c_str(), "ResultsTree");*/
    /*resultFile.SaveMultipleValues(detectors,
     * generatedData.allParticlesTheoreticalStates[i],
     * generatedData.allParticlesRealStates[i], allParticlesMeasures[i],
     * predictedStates, filteredStates, smoothedStates);*/
  }
  Utils::saveDataToCSV(detectors, generatedData.allParticlesTheoreticalStates,
                       generatedData.allParticlesRealStates,
                       allParticlesMeasures, allParticlesPredictedStates,
                       allParticlesFilteredStates, allParticlesSmoothedStates,
                       runCounter);
  runCounter++;
}

void Simulation::testDetector(int particlesNumber, int detectorId) {
  // DATA CREATION
  GeneratedData generatedData =
      dataGenerator.generateAllData(particlesNumber, false, true);
  std::vector<Measurement> allMeasures =
      Utils::concatenateMeasures(generatedData.allParticlesMeasures);

  // DATA SAVING
  std::string dataFileName =
      "../data/GeneratedData_run" + std::to_string(runCounter) + ".root";
  DataFile dataFile = DataFile(dataFileName.c_str(), "DataTree", false);
  dataFile.SaveMultipleMeasures(allMeasures);

  // DATA ELABORATION
  allMeasures = dataFile.readMeasures();
  std::vector<std::vector<Measurement>> allParticlesMeasures =
      Utils::separateMeasuresInParticles(allMeasures);

  tracker.ignoreDetector(detectorId);
  std::vector<std::vector<MatrixStateEstimate>> allParticlesSmoothedStates;
  for (int i = 0; i < (int)allParticlesMeasures.size(); i++) {
    std::vector<Measurement> givenMeasures = allParticlesMeasures[i];
    const Measurement detectorMeasurement = givenMeasures[detectorId];
    givenMeasures.erase(givenMeasures.begin() + detectorId);

    kalmanFilterResult filterResults =
        tracker.kalmanFilter(givenMeasures, false, false);
    std::vector<MatrixStateEstimate> predictedStates =
        filterResults.predictedStates;
    std::vector<MatrixStateEstimate> filteredStates =
        filterResults.filteredStates;

    std::vector<MatrixStateEstimate> smoothedStates =
        tracker.kalmanSmoother(filteredStates, false);

    MatrixStateEstimate preaviousStateEstimate = smoothedStates[detectorId];
    double deltaZ =
        (detectorId == 0)
            ? detectors[detectorId].getBottmLeftPosition().Z()
            : detectors[detectorId].getBottmLeftPosition().Z() -
                  detectors[detectorId - 1].getBottmLeftPosition().Z();
    MatrixStateEstimate estimatedNextState =
        tracker.estimateNextState(preaviousStateEstimate, deltaZ);
    TMatrixD estimatedValue = estimatedNextState.value;
    TMatrixD estimatedError = estimatedNextState.uncertainty;

    std::cout << "DIFFERENCE CALCULATED AT THE DETECTOR WITH ID " << detectorId
              << std::endl;
    std::cout << "Detector measurement: t=" << detectorMeasurement.t << "±"
              << DETECTOR_TIME_UNCERTAINTY
              << " |   x = " << detectorMeasurement.x << "±"
              << DETECTOR_SPACE_UNCERTAINTY
              << " |   y =" << detectorMeasurement.y << "±"
              << DETECTOR_SPACE_UNCERTAINTY << std::endl;
    std::cout << "Smoother estimate: t=" << estimatedValue(0, 0) << "±"
              << sqrt(estimatedError(0, 0))
              << " |   x = " << estimatedValue(1, 0) << "±"
              << sqrt(estimatedError(1, 1))
              << " |   y = " << estimatedValue(2, 0) << "±"
              << sqrt(estimatedError(2, 2)) << std::endl;

    double Zt = (detectorMeasurement.t - estimatedValue(0, 0)) /
                sqrt(DETECTOR_TIME_UNCERTAINTY * DETECTOR_TIME_UNCERTAINTY +
                     estimatedError(0, 0));
    double Zx = (detectorMeasurement.x - estimatedValue(1, 0)) /
                sqrt(DETECTOR_SPACE_UNCERTAINTY * DETECTOR_SPACE_UNCERTAINTY +
                     estimatedError(1, 1));
    double Zy = (detectorMeasurement.y - estimatedValue(2, 0)) /
                sqrt(DETECTOR_SPACE_UNCERTAINTY * DETECTOR_SPACE_UNCERTAINTY +
                     estimatedError(2, 2));
    std::cout << "Z_t = " << Zt << "    Z_x = " << Zx << "    Z_y = " << Zy
              << std::endl;

    smoothedStates.insert(smoothedStates.begin() + detectorId + 1,
                          estimatedNextState);
    allParticlesSmoothedStates.push_back(smoothedStates);
  }
  tracker.resetDetectors();
  Utils::saveDataToCSV(detectors, generatedData.allParticlesRealStates,
                       allParticlesMeasures, allParticlesSmoothedStates,
                       runCounter);
  runCounter++;
}
