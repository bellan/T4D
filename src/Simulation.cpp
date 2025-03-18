// Header files needed
#include <TFile.h>
#include <TLorentzVector.h>
#include <TMatrixD.h>
#include <TMatrixDfwd.h>
#include <TTree.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <TVector3.h>
#include <TClonesArray.h>

// Custom classes
#include "Simulation.hpp"
#include "DataFile.hpp"
#include "DataGenerator.hpp"
#include "PhysicalParameters.hpp"
#include "SetupFactory.hpp"
#include "Structs.hpp"
#include "ParticleGun.hpp"
#include "ParticleState.hpp"
#include "Tracker.hpp"
#include "Utils.hpp"

// Namespaces
using namespace std;


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Global class counters
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Simulation counter
int Simulation::runCounter = 0;

Simulation::~Simulation() {}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Simulation (original constructor)
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
// Simulation (constructor)
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Simulation::Simulation(vector<Detector> detectors) 
: detectors(detectors) {
  // to be removed when everything is fine with the new system
  SetupFactory factory{};
  const SimulationSetup experiment = factory.generateExperiment();
  //detectors = experiment.detectors;
  dataGenerator = DataGenerator(experiment);
  //tracker = Tracker(experiment.detectors);
  tracker = Tracker(detectors);

  if (detectors.size() == 0) {
    throw std::invalid_argument("No detector found");
  }

  // --- Output file
  TFile file_out = TFile("../data/Simulation.root", "RECREATE");
  if(file_out.IsZombie()){
    cout << "Problem in creating the simulation output file." << endl;
  }

  file_out.cd();

  // --- Simulation tree
  // Tree for generated particles
  TTree tree_generated = TTree("tree_generation", "Simulation tree with generated particles");

  // Particle gun branch
  tree_generated.Branch("ParticleGun", &data_generated.gun);
  tree_generated.Branch("Layer0_particles", &data_generated.lay0_particles); // Particles' initial direction and velocity
  tree_generated.Branch("Layer1_particles", &data_generated.lay1_particles);
  tree_generated.Branch("Layer2_particles", &data_generated.lay2_particles);
  tree_generated.Branch("Layer3_particles", &data_generated.lay3_particles);
  tree_generated.Branch("Layer4_particles", &data_generated.lay4_particles);
  tree_generated.Branch("Layer5_particles", &data_generated.lay5_particles);
  tree_generated.Branch("Layer6_particles", &data_generated.lay6_particles);
  tree_generated.Branch("Layer7_particles", &data_generated.lay7_particles);
  tree_generated.Branch("Layer8_particles", &data_generated.lay8_particles);


  // --- Detector response tree
  TTree tree_detector = TTree("tree_detector", "Simulation tree with particles after the simulation of detector response");

  // Particle gun branch
  //tree_detector -> Branch("ParticleGun", &data_detector.gun);
  tree_detector.Branch("Layer1_particles", &data_detector.lay1_particles);
  tree_detector.Branch("Layer2_particles", &data_detector.lay2_particles);
  tree_detector.Branch("Layer3_particles", &data_detector.lay3_particles);
  tree_detector.Branch("Layer4_particles", &data_detector.lay4_particles);
  tree_detector.Branch("Layer5_particles", &data_detector.lay5_particles);
  tree_detector.Branch("Layer6_particles", &data_detector.lay6_particles);
  tree_detector.Branch("Layer7_particles", &data_detector.lay7_particles);
  tree_detector.Branch("Layer8_particles", &data_detector.lay8_particles);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Generation
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool Simulation::Generation() {
  // Declaring variables
  vector<Particle> allParticles;
  vector<ParticleState> particle_states;
  particle_states.reserve(NUMBER_OF_DETECTORS + 1);
  ParticleState newState;

  // Loop on numner of events
  for (unsigned int e = 0; e < NUMBER_OF_EVENTS; e++) {
    // Particle gun
    data_generated.gun = ParticleGun({0,0,0}, this->detectors, 0);

    // --- Generation of particle and their path in the detectors
    for (unsigned int p = 0; p < NUMBER_OF_PARTICLES; p++){
      // Creation of the particle
      Particle particle = data_generated.gun.generateParticle(p);

      // -- Propagation across the detectors
      // State at vertex
      particle_states.push_back(particle.getInitialState());

      // States at detectors
      for (const Detector &detector : this->detectors) {
        newState = particle.zSpaceEvolve(particle_states.back(), detector.getBottmLeftPosition().z(), false, detector.getId());
        particle_states.push_back(newState);
      }

      // -- Saving the hits in the tree
      data_generated.lay0_particles.push_back(particle_states.at(0));
      data_generated.lay1_particles.push_back(particle_states.at(1));
      data_generated.lay2_particles.push_back(particle_states.at(2));
      data_generated.lay3_particles.push_back(particle_states.at(3));
      data_generated.lay4_particles.push_back(particle_states.at(4));
      data_generated.lay5_particles.push_back(particle_states.at(5));
      data_generated.lay6_particles.push_back(particle_states.at(6));
      data_generated.lay7_particles.push_back(particle_states.at(7));
      data_generated.lay8_particles.push_back(particle_states.at(8));
    }

    // Filling the tree
    tree_generated.Fill();

    // Clear vectors
    particle_states.clear();
    data_generated.lay0_particles.clear();
    data_generated.lay1_particles.clear();
    data_generated.lay2_particles.clear();
    data_generated.lay3_particles.clear();
    data_generated.lay4_particles.clear();
    data_generated.lay5_particles.clear();
    data_generated.lay6_particles.clear();
    data_generated.lay7_particles.clear();
    data_generated.lay8_particles.clear();
  }

  tree_generated.Write();
  return true;
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

  // /*
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

  for (size_t i = 0; i < allParticlesMeasures.size(); i++) {
    // Kalman filter
    kalmanFilterResult filterResults = tracker.kalmanFilter(allParticlesMeasures[i], false, false);
    vector<MatrixStateEstimate> predictedStates = filterResults.predictedStates;
    vector<MatrixStateEstimate> filteredStates = filterResults.filteredStates;

    // Kalman smoother
    vector<MatrixStateEstimate> smoothedStates = tracker.kalmanSmoother(filteredStates, false);
    allParticlesPredictedStates.push_back(predictedStates);
    allParticlesFilteredStates.push_back(filteredStates);
    allParticlesSmoothedStates.push_back(smoothedStates);
    

    //cout << "Filtered Chi2" << endl;
    //tracker.computeChi2s(generatedData.allParticlesRealStates[i],
    //                     filteredStates, true, true);
    //cout << "\nSmoothed Chi2" << endl;
    //tracker.computeChi2s(generatedData.allParticlesRealStates[i],
    //                     smoothedStates, true, true);

    //string fileName("../results/Particle ");
    //fileName += to_string(i);
    //fileName += ".root";
    
    //ResultFile resultFile(fileName.c_str(), "ResultsTree");
    //resultFile.SaveMultipleValues(detectors,
    // generatedData.allParticlesTheoreticalStates[i],
    // generatedData.allParticlesRealStates[i], allParticlesMeasures[i],
    // predictedStates, filteredStates, smoothedStates);
    
  }
  

  // --- Data export
  Utils::saveDataToCSV(detectors, generatedData.allParticlesTheoreticalStates, generatedData.allParticlesRealStates,
                       allParticlesMeasures, allParticlesPredictedStates, allParticlesFilteredStates, allParticlesSmoothedStates,
                       runCounter);
  // */
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
