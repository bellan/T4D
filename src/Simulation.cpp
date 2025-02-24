// Header files needed
#include <TFile.h>
#include <TLorentzVector.h>
#include <TMatrixD.h>
#include <TMatrixDfwd.h>
#include <stdexcept>
#include <string>
#include <vector>

// Custom classes
#include "Simulation.hpp"
#include "DataFile.hpp"
#include "DataGenerator.hpp"
#include "MeasuresAndStates.hpp"
#include "PhysicalParameters.hpp"
#include "SetupFactory.hpp"
#include "Tracker.hpp"
#include "Utils.hpp"

// Namespaces
using namespace std;



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Global class counters
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Simulation counter
int Simulation::runCounter = 0;



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Simulation (constructor)
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Simulation::Simulation() 
: detectors() {
  SetupFactory factory{};
  const SimulationSetup experiment = factory.generateExperiment();
  detectors = experiment.detectors;
  dataGenerator = DataGenerator(experiment);
  tracker = Tracker(experiment.detectors);

  if (detectors.size() == 0) {
    throw std::invalid_argument("No detector");
  }
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// runSimulation
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Simulation::runSimulation(int particlesNumber) {
  // --- Data creation
  GeneratedData generatedData = dataGenerator.generateAllData(particlesNumber, false, true);
  vector<Measurement> allMeasures = Utils::concatenateMeasures(generatedData.allParticlesMeasures);

  // --- Data saving
  string dataFileName = "../data/GeneratedData_run" + to_string(runCounter) + ".root";
  DataFile dataFile = DataFile(dataFileName.c_str(), "DataTree", false);
  dataFile.SaveMultipleMeasures(allMeasures);

  // --- Data elaboration
  allMeasures = dataFile.readMeasures();
  vector<vector<Measurement>> allParticlesMeasures = Utils::separateMeasuresInParticles(allMeasures);

  // Vector for reconstructing the track
  vector<vector<MatrixStateEstimate>> allParticlesPredictedStates;
  allParticlesPredictedStates.reserve(particlesNumber);
  vector<vector<MatrixStateEstimate>> allParticlesFilteredStates;
  allParticlesFilteredStates.reserve(particlesNumber);
  vector<vector<MatrixStateEstimate>> allParticlesSmoothedStates;
  allParticlesSmoothedStates.reserve(particlesNumber);

  for (int i = 0; i < (int)allParticlesMeasures.size(); i++) {
    // Kalman filter
    kalmanFilterResult filterResults = tracker.kalmanFilter(allParticlesMeasures[i], false, false);
    vector<MatrixStateEstimate> predictedStates = filterResults.predictedStates;
    vector<MatrixStateEstimate> filteredStates = filterResults.filteredStates;

    // Kalman smoother
    vector<MatrixStateEstimate> smoothedStates = tracker.kalmanSmoother(filteredStates, false);
    allParticlesPredictedStates.push_back(predictedStates);
    allParticlesFilteredStates.push_back(filteredStates);
    allParticlesSmoothedStates.push_back(smoothedStates);

    /*cout << "Filtered Chi2" << endl;*/
    /*tracker.computeChi2s(generatedData.allParticlesRealStates[i],*/
    /*                     filteredStates, true, true);*/
    /*cout << "\nSmoothed Chi2" << endl;*/
    /*tracker.computeChi2s(generatedData.allParticlesRealStates[i],*/
    /*                     smoothedStates, true, true);*/

    /*string fileName("../results/Particle ");*/
    /*fileName += to_string(i);*/
    /*fileName += ".root";*/
    /**/
    /*ResultFile resultFile(fileName.c_str(), "ResultsTree");*/
    /*resultFile.SaveMultipleValues(detectors,
     * generatedData.allParticlesTheoreticalStates[i],
     * generatedData.allParticlesRealStates[i], allParticlesMeasures[i],
     * predictedStates, filteredStates, smoothedStates);*/
  }

  // --- Data export
  Utils::saveDataToCSV(detectors, generatedData.allParticlesTheoreticalStates, generatedData.allParticlesRealStates,
                       allParticlesMeasures, allParticlesPredictedStates, allParticlesFilteredStates, allParticlesSmoothedStates,
                       runCounter);
  runCounter++;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// testDetector
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Simulation::testDetector(int particlesNumber, int detectorId) {
  // Data creation
  GeneratedData generatedData = dataGenerator.generateAllData(particlesNumber, false, true);
  vector<Measurement> allMeasures = Utils::concatenateMeasures(generatedData.allParticlesMeasures);

  // Data saving
  string dataFileName = "../data/GeneratedData_run" + to_string(runCounter) + ".root";
  DataFile dataFile = DataFile(dataFileName.c_str(), "DataTree", false);
  dataFile.SaveMultipleMeasures(allMeasures);

  // Data elaboration
  allMeasures = dataFile.readMeasures();
  vector<vector<Measurement>> allParticlesMeasures = Utils::separateMeasuresInParticles(allMeasures);

  tracker.ignoreDetector(detectorId);
  vector<vector<MatrixStateEstimate>> allParticlesSmoothedStates;

  for (int i = 0; i < (int)allParticlesMeasures.size(); i++) {
    vector<Measurement> givenMeasures = allParticlesMeasures[i];
    const Measurement detectorMeasurement = givenMeasures[detectorId];
    givenMeasures.erase(givenMeasures.begin() + detectorId);

    kalmanFilterResult filterResults = tracker.kalmanFilter(givenMeasures, false, false);
    vector<MatrixStateEstimate> predictedStates = filterResults.predictedStates;
    vector<MatrixStateEstimate> filteredStates = filterResults.filteredStates;

    vector<MatrixStateEstimate> smoothedStates = tracker.kalmanSmoother(filteredStates, false);

    MatrixStateEstimate preaviousStateEstimate = smoothedStates[detectorId];
    double deltaZ =
        (detectorId == 0)
            ? detectors[detectorId].getBottmLeftPosition().Z()
            : detectors[detectorId].getBottmLeftPosition().Z() - detectors[detectorId - 1].getBottmLeftPosition().Z();
    
    MatrixStateEstimate estimatedNextState = tracker.estimateNextState(preaviousStateEstimate, deltaZ);
    TMatrixD estimatedValue = estimatedNextState.value;
    TMatrixD estimatedError = estimatedNextState.uncertainty;

    cout << "DIFFERENCE CALCULATED AT THE DETECTOR WITH ID " << detectorId << endl;

    cout << "Detector measurement: t=" << detectorMeasurement.t << "±"<< DETECTOR_TIME_UNCERTAINTY
              << " |   x = " << detectorMeasurement.x << "±" << DETECTOR_SPACE_UNCERTAINTY
              << " |   y =" << detectorMeasurement.y << "±" << DETECTOR_SPACE_UNCERTAINTY << endl;

    cout << "Smoother estimate: t=" << estimatedValue(0, 0) << "±" << sqrt(estimatedError(0, 0))
              << " |   x = " << estimatedValue(1, 0) << "±" << sqrt(estimatedError(1, 1))
              << " |   y = " << estimatedValue(2, 0) << "±" << sqrt(estimatedError(2, 2)) << endl;

    double Zt = (detectorMeasurement.t - estimatedValue(0, 0)) / sqrt(DETECTOR_TIME_UNCERTAINTY * DETECTOR_TIME_UNCERTAINTY + estimatedError(0, 0));
    double Zx = (detectorMeasurement.x - estimatedValue(1, 0)) / sqrt(DETECTOR_SPACE_UNCERTAINTY * DETECTOR_SPACE_UNCERTAINTY + estimatedError(1, 1));
    double Zy = (detectorMeasurement.y - estimatedValue(2, 0)) / sqrt(DETECTOR_SPACE_UNCERTAINTY * DETECTOR_SPACE_UNCERTAINTY + estimatedError(2, 2));

    cout << "Z_t = " << Zt << "    Z_x = " << Zx << "    Z_y = " << Zy << endl;

    smoothedStates.insert(smoothedStates.begin() + detectorId + 1, estimatedNextState);
    allParticlesSmoothedStates.push_back(smoothedStates);
  }

  // --- Data export
  tracker.resetDetectors();
  Utils::saveDataToCSV(detectors, generatedData.allParticlesRealStates, allParticlesMeasures, allParticlesSmoothedStates, runCounter);
  runCounter++;
}
