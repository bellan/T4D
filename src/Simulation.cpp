// Header files needed
#include <TFile.h>
#include <TLorentzVector.h>
#include <TMatrixD.h> // TO BE REMOVED after code separation
#include <TMatrixDfwd.h> // TO BE REMOVED after code separation
#include <TTree.h>
#include <stdexcept> // ??? TO BE REMOVED after code separation 
#include <string> // ??? TO BE REMOVED after code separation
#include <vector>
#include <TVector3.h> // ??? TO BE REMOVED after code separation
#include <TClonesArray.h> // TO BE REMOVED after code separation

// Custom classes
#include "Simulation.hpp"
#include "DataFile.hpp" // TO BE REMOVED after code separation
#include "DataGenerator.hpp" // TO BE REMOVED after code separation
#include "Measure.hpp"
#include "PhysicalParameters.hpp"
#include "SetupFactory.hpp" // TO BE REMOVED after code separation
#include "Structs.hpp"
#include "ParticleGun.hpp"
#include "ParticleState.hpp"
#include "Tracker.hpp" // TO BE REMOVED after code separation
#include "Utils.hpp" // TO BE REMOVED after code separation

// Namespaces
using namespace std;


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Global class counters
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Simulation counter
int Simulation::runCounter = 0;



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~Simulation (destructor)
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Simulation::~Simulation() {
  delete tree_generated;
  delete tree_detector;
  delete tree_measures;

  file_out.Close();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Simulation (original constructor)
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*
Simulation::Simulation() 
: detectors() {
  SetupFactory factory{};
  const SimulationSetup experiment = factory.generateExperiment();
  detectors = experiment.detectors;
  dataGenerator = DataGenerator(experiment);
  tracker = Tracker(detectors);

  if (detectors.size() == 0) {
    throw std::invalid_argument("No detector");
  }
}
*/



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Simulation (constructor)
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Simulation::Simulation(const vector<Detector>& detectors) 
: file_out("../data/Simulation.root", "RECREATE"),
  detectors(detectors)
{
  // to be removed when everything is fine with the new system
  //SetupFactory factory{};
  //const SimulationSetup experiment = factory.generateExperiment();
  //detectors = experiment.detectors;
  //dataGenerator = DataGenerator(experiment);
  //tracker = Tracker(experiment.detectors);
  //tracker = Tracker(detectors);
  // end of removal


  // --- Detectors
  if (detectors.size() == 0) {
    throw std::invalid_argument("No detector found.");
  }


  // --- Output file
  if(file_out.IsZombie()){
    throw std::invalid_argument("Problem in creating the simulation output file.");
  }

  file_out.cd();

  // Planting the trees in the output file
  tree_generated = new TTree("tree_generation", "Simulation tree with generated particles");
  tree_detector = new TTree("tree_detector", "Simulation tree with particles after the simulation of detector response");
  tree_measures = new TTree("tree_measures", "Simulation tree with the coordinates after the measurement simulation");


  // --- Generation tree
  // Branch for the particle gun
  tree_generated -> Branch("ParticleGun", &data_generated.gun);
  // Branches for the layers of the detectors
  tree_generated -> Branch("Layer0_particles_gen", &data_generated.lay0_particles); // Particles' initial direction and velocity
  tree_generated -> Branch("Layer1_particles_gen", &data_generated.lay1_particles);
  tree_generated -> Branch("Layer2_particles_gen", &data_generated.lay2_particles);
  tree_generated -> Branch("Layer3_particles_gen", &data_generated.lay3_particles);
  tree_generated -> Branch("Layer4_particles_gen", &data_generated.lay4_particles);
  tree_generated -> Branch("Layer5_particles_gen", &data_generated.lay5_particles);
  tree_generated -> Branch("Layer6_particles_gen", &data_generated.lay6_particles);
  tree_generated -> Branch("Layer7_particles_gen", &data_generated.lay7_particles);
  tree_generated -> Branch("Layer8_particles_gen", &data_generated.lay8_particles);


  // --- Detector response tree
  // Branch for the particle gun
  //tree_detector -> Branch("ParticleGun", &data_detector.gun);
  // Branches for the layers of the detectors
  tree_detector -> Branch("Layer1_particles_dr", &data_detector.lay1_particles);
  tree_detector -> Branch("Layer2_particles_dr", &data_detector.lay2_particles);
  tree_detector -> Branch("Layer3_particles_dr", &data_detector.lay3_particles);
  tree_detector -> Branch("Layer4_particles_dr", &data_detector.lay4_particles);
  tree_detector -> Branch("Layer5_particles_dr", &data_detector.lay5_particles);
  tree_detector -> Branch("Layer6_particles_dr", &data_detector.lay6_particles);
  tree_detector -> Branch("Layer7_particles_dr", &data_detector.lay7_particles);
  tree_detector -> Branch("Layer8_particles_dr", &data_detector.lay8_particles);


  // --- Measures tree
  // Branches for the layers of the detectors
  tree_measures -> Branch("Layer1_particles_mea", &data_measures.lay1_particles);
  tree_measures -> Branch("Layer2_particles_mea", &data_measures.lay2_particles);
  tree_measures -> Branch("Layer3_particles_mea", &data_measures.lay3_particles);
  tree_measures -> Branch("Layer4_particles_mea", &data_measures.lay4_particles);
  tree_measures -> Branch("Layer5_particles_mea", &data_measures.lay5_particles);
  tree_measures -> Branch("Layer6_particles_mea", &data_measures.lay6_particles);
  tree_measures -> Branch("Layer7_particles_mea", &data_measures.lay7_particles);
  tree_measures -> Branch("Layer8_particles_mea", &data_measures.lay8_particles);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Generation
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool Simulation::Generation() {
  // Declaring variables
  bool end_success = false;
  vector<ParticleState> particle_states;
  particle_states.reserve(NUMBER_OF_DETECTORS + 1);
  ParticleState newState;

  // --- Loop on number of events
  for (unsigned int e = 0; e < NUMBER_OF_EVENTS; e++) {
    // Particle gun
    data_generated.gun = ParticleGun({0,0,0}, this->detectors, 0.);

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

      // -- Adding the hits to the tree branches
      data_generated.lay0_particles.push_back(particle_states[0]);
      data_generated.lay1_particles.push_back(particle_states[1]);
      data_generated.lay2_particles.push_back(particle_states[2]);
      data_generated.lay3_particles.push_back(particle_states[3]);
      data_generated.lay4_particles.push_back(particle_states[4]);
      data_generated.lay5_particles.push_back(particle_states[5]);
      data_generated.lay6_particles.push_back(particle_states[6]);
      data_generated.lay7_particles.push_back(particle_states[7]);
      data_generated.lay8_particles.push_back(particle_states[8]);

      // -- Cleaning vectors
      particle_states.clear();
    }

    // Filling the tree
    tree_generated -> Fill();

    // Cleaning vectors
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

  // Writing the tree
  tree_generated -> Write();

  end_success = true;
  return end_success;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Detector response
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool Simulation::DetectorResponse() {
  // Declaring variables
  bool end_success = false;
  vector<ParticleState> particle_states;
  particle_states.reserve(NUMBER_OF_DETECTORS + 1);
  ParticleState newState;

  // Check on the number of events
  if (tree_generated -> GetEntries() != NUMBER_OF_EVENTS) {
    cerr << " [ERROR] tree_generated has " << tree_generated -> GetEntries() << " events, but " << NUMBER_OF_EVENTS << " were expected." << endl;
    return end_success;
  }

  // --- Loop on number of events
  for (unsigned int e = 0; e < NUMBER_OF_EVENTS; e++) {
    // Load the e-th event from the generation tree
    if (tree_generated -> GetEntry(e) <= 0) {
      cerr << "[ERROR] Unable to read entry " << e << " from tree_detector." << endl;
      return end_success;
    }

    // --- Loop on particles of the event
    for (const ParticleState& initial : data_generated.lay0_particles) {
      // Create the particle from initial state
      Particle particle(initial);

      // -- Propagation across the detectors
      // State at vertex with generated initial velocity and direction
      particle_states.push_back(initial);

      // States at detectors
      for (const Detector& detector : detectors) {
        newState = particle.zSpaceEvolve(particle_states.back(), detector.getBottmLeftPosition().z(), true, detector.getId());
        particle_states.push_back(newState);
      }

      // -- Adding the hits to the tree branches
      data_detector.lay1_particles.push_back(particle_states[1]);
      data_detector.lay2_particles.push_back(particle_states[2]);
      data_detector.lay3_particles.push_back(particle_states[3]);
      data_detector.lay4_particles.push_back(particle_states[4]);
      data_detector.lay5_particles.push_back(particle_states[5]);
      data_detector.lay6_particles.push_back(particle_states[6]);
      data_detector.lay7_particles.push_back(particle_states[7]);
      data_detector.lay8_particles.push_back(particle_states[8]);

      // -- Cleaning vectors
      particle_states.clear();
    }

    // Filling the tree
    tree_detector -> Fill();

    // Cleaning vectors
    particle_states.clear();
    data_detector.lay1_particles.clear();
    data_detector.lay2_particles.clear();
    data_detector.lay3_particles.clear();
    data_detector.lay4_particles.clear();
    data_detector.lay5_particles.clear();
    data_detector.lay6_particles.clear();
    data_detector.lay7_particles.clear();
    data_detector.lay8_particles.clear();
  }

  // Writing the tree
  tree_detector -> Write();

  end_success = true;
  return end_success;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Measurement
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool Simulation::Measurement() {
  // Declaring variables
  bool end_success = false;
  vector<ParticleState>* detector_particles = nullptr;

  // Check on the number of events
  if (tree_detector -> GetEntries() != NUMBER_OF_EVENTS) {
    cerr << " [ERROR] tree_generated has " << tree_generated -> GetEntries() << " events, but " << NUMBER_OF_EVENTS << " were expected." << endl;
    return end_success;
  }

  // --- Loop on particles of the event
  for (unsigned int e = 0; e < NUMBER_OF_EVENTS; ++e) {
    if (tree_detector -> GetEntry(e) <= 0) {
      cerr << "[ERROR] Unable to read entry " << e << " from tree_detector." << endl;
      return false;
    }

    // --- Loop on detectors
    for (int d = 1; d < NUMBER_OF_DETECTORS + 1; d++) {
      // -- Getting the particle states for each detector
      switch (d) {
        case 1: detector_particles = &data_detector.lay1_particles; break;
        case 2: detector_particles = &data_detector.lay2_particles; break;
        case 3: detector_particles = &data_detector.lay3_particles; break;
        case 4: detector_particles = &data_detector.lay4_particles; break;
        case 5: detector_particles = &data_detector.lay5_particles; break;
        case 6: detector_particles = &data_detector.lay6_particles; break;
        case 7: detector_particles = &data_detector.lay7_particles; break;
        case 8: detector_particles = &data_detector.lay8_particles; break;
      }

      // If there are not particles, continue to the next detector
      if (!detector_particles) continue;

      // -- Loop on the particle states on each detector
      for (const ParticleState& state : *detector_particles) {
        // Getting the (d-1)-th detector because in the vector of the detectors, the particle gun is not included:
        // the IDs of the detectors are 1-8, however their positions in the vector are 0-7 
        const Detector& detector = this->detectors.at(d-1);

        // Simulating the measurement for the state
        std::optional<Measure> state_measure = detector.measure2(state.position, state.particleID);

        // Putting the measures in the correct branch of the event
        if (state_measure) {
          switch (d) {
            case 1: data_measures.lay1_particles.push_back(*state_measure); break;
            case 2: data_measures.lay2_particles.push_back(*state_measure); break;
            case 3: data_measures.lay3_particles.push_back(*state_measure); break;
            case 4: data_measures.lay4_particles.push_back(*state_measure); break;
            case 5: data_measures.lay5_particles.push_back(*state_measure); break;
            case 6: data_measures.lay6_particles.push_back(*state_measure); break;
            case 7: data_measures.lay7_particles.push_back(*state_measure); break;
            case 8: data_measures.lay8_particles.push_back(*state_measure); break;
          }
        }
      }

      // Cleaning vectors
      detector_particles -> clear();
    }

    // Filling the tree
    tree_measures -> Fill();

    // Cleaning vectors
    data_measures.lay1_particles.clear();
    data_measures.lay2_particles.clear();
    data_measures.lay3_particles.clear();
    data_measures.lay4_particles.clear();
    data_measures.lay5_particles.clear();
    data_measures.lay6_particles.clear();
    data_measures.lay7_particles.clear();
    data_measures.lay8_particles.clear();
  }

  // Writing the tree
  tree_measures -> Write();

  end_success = true;
  return end_success;
}


// OLD -----------------------------------------------------------------------

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// runSimulation
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*
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
  //
  runCounter++;
}
*/


/*
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
*/