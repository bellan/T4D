// Header files needed
#include <cmath>
#include <string>
#include <TFile.h>
#include <TMatrixD.h>
#include <TMatrixDfwd.h>
#include <TTree.h>
#include <vector>

// Custom classes
#include "Detector.hpp"
#include "Measure.hpp"
#include "ParticleState.hpp"
#include "PhysicalParameters.hpp"
#include "Simulation.hpp"
#include "Structs.hpp"
#include "Tracker.hpp"
#include "Utils.hpp" // TO BE REMOVED after code separation
#include "MatrixEstimate.hpp"
#include "StructMeasures.hpp"

// Namespaces
using namespace std;



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Global variables
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Initializing the state at 0, that is at the particle cannon 6 dimentional vector (t, x, y, 1/speed, thetazx, thetazy)
// Initial state
static constexpr double initialStateData[6] = {0., 0., 0., 1. / LIGHT_SPEED, 0., 0.};
static const TMatrixD initialStateValue(6, 1, initialStateData);

// Initial state uncertainties
static constexpr double bigT = VERY_HIGH_TIME_ERROR * VERY_HIGH_TIME_ERROR;
static constexpr double bigX = VERY_HIGH_SPACE_ERROR * VERY_HIGH_SPACE_ERROR;
static constexpr double bigVInv = VERY_HIGH_VELOCITY_INVERSE_ERROR * VERY_HIGH_VELOCITY_INVERSE_ERROR;
static constexpr double bigDirection = VERY_HIGH_DIRECTION_ERROR * VERY_HIGH_DIRECTION_ERROR;

static constexpr double initialStateSData[36] = {
    bigT, 0., 0., 0., 0., 0.,
    0., bigX, 0., 0., 0., 0.,
    0., 0., bigX, 0., 0., 0.,
    0., 0., 0., bigVInv, 0., 0.,
    0., 0., 0., 0., bigDirection, 0.,
    0., 0., 0., 0., 0., bigDirection};

static const TMatrixD initialStateError(6, 6, initialStateSData);

// Initial state estimated
static const MatrixStateEstimate initialState{initialStateValue, initialStateError};



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Tracker (constructor)
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Tracker::Tracker(const vector<Detector>& detectors, string& path_fin, string& path_fout) 
  : file_in(path_fin.c_str(), "READ"),
    file_out(path_fout.c_str(), "RECREATE"),
    detectors(detectors),
    consideredDetectors(detectors)
{
  // --- Reading measurements from file
  // Opening input file
  if (file_in.IsZombie()) {
    cerr << "Error: Cannot open input file " << path_fin << endl;
    throw std::runtime_error("Input file not found or corrupted");
  }

  // Linking the tree and its branches
  TTree* tree_temp = nullptr;
  file_in.GetObject("tree_measures", tree_temp);
  tree_measures = tree_temp;

  if (!tree_measures){
    cerr << "Error: TTree 'tree_measures' not found in input file." << endl;
    file_in.ls();
    throw std::runtime_error("Missing input tree");
  }

  // Measures tree
  tree_measures -> SetBranchAddress("Layer1_particles_mea", &data_measures.lay1_particles);
  tree_measures -> SetBranchAddress("Layer2_particles_mea", &data_measures.lay2_particles);
  tree_measures -> SetBranchAddress("Layer3_particles_mea", &data_measures.lay3_particles);
  tree_measures -> SetBranchAddress("Layer4_particles_mea", &data_measures.lay4_particles);
  tree_measures -> SetBranchAddress("Layer5_particles_mea", &data_measures.lay5_particles);
  tree_measures -> SetBranchAddress("Layer6_particles_mea", &data_measures.lay6_particles);
  tree_measures -> SetBranchAddress("Layer7_particles_mea", &data_measures.lay7_particles);
  tree_measures -> SetBranchAddress("Layer8_particles_mea", &data_measures.lay8_particles);


  // --- Output file
  if(file_out.IsZombie()){
    throw std::invalid_argument("Problem in creating the simulation output file.");
  }

  // Planting the trees in the output files
  file_out.cd();
  tree_predicted = new TTree("tree_predicted", "Kalman filter tree with predicted states");
  tree_filtered = new TTree("tree_filtered", "Kalman filter tree with filtered states");
  tree_smoothed = new TTree("tree_smoothed", "Kalman filter tree with smoothed states");

  // -- Predicted states tree
  // Branches
  tree_predicted -> Branch("Layer0_particles_pre", &filter_predicted.lay0_particles); // Initial state of the filter
  tree_predicted -> Branch("Layer1_particles_pre", &filter_predicted.lay1_particles);
  tree_predicted -> Branch("Layer2_particles_pre", &filter_predicted.lay2_particles);
  tree_predicted -> Branch("Layer3_particles_pre", &filter_predicted.lay3_particles);
  tree_predicted -> Branch("Layer4_particles_pre", &filter_predicted.lay4_particles);
  tree_predicted -> Branch("Layer5_particles_pre", &filter_predicted.lay5_particles);
  tree_predicted -> Branch("Layer6_particles_pre", &filter_predicted.lay6_particles);
  tree_predicted -> Branch("Layer7_particles_pre", &filter_predicted.lay7_particles);
  tree_predicted -> Branch("Layer8_particles_pre", &filter_predicted.lay8_particles);

  tree_predicted -> Branch("Layer0_errors_pre", &filter_spredicted.lay0_particles); // Initial state of the filter
  tree_predicted -> Branch("Layer1_errors_pre", &filter_spredicted.lay1_particles);
  tree_predicted -> Branch("Layer2_errors_pre", &filter_spredicted.lay2_particles);
  tree_predicted -> Branch("Layer3_errors_pre", &filter_spredicted.lay3_particles);
  tree_predicted -> Branch("Layer4_errors_pre", &filter_spredicted.lay4_particles);
  tree_predicted -> Branch("Layer5_errors_pre", &filter_spredicted.lay5_particles);
  tree_predicted -> Branch("Layer6_errors_pre", &filter_spredicted.lay6_particles);
  tree_predicted -> Branch("Layer7_errors_pre", &filter_spredicted.lay7_particles);
  tree_predicted -> Branch("Layer8_errors_pre", &filter_spredicted.lay8_particles);

  // -- Filtered states tree
  // Branches
  tree_filtered -> Branch("Layer0_particles_fil", &filter_filtered.lay0_particles); // Initial state of the filter
  tree_filtered -> Branch("Layer1_particles_fil", &filter_filtered.lay1_particles);
  tree_filtered -> Branch("Layer2_particles_fil", &filter_filtered.lay2_particles);
  tree_filtered -> Branch("Layer3_particles_fil", &filter_filtered.lay3_particles);
  tree_filtered -> Branch("Layer4_particles_fil", &filter_filtered.lay4_particles);
  tree_filtered -> Branch("Layer5_particles_fil", &filter_filtered.lay5_particles);
  tree_filtered -> Branch("Layer6_particles_fil", &filter_filtered.lay6_particles);
  tree_filtered -> Branch("Layer7_particles_fil", &filter_filtered.lay7_particles);
  tree_filtered -> Branch("Layer8_particles_fil", &filter_filtered.lay8_particles);

  tree_filtered -> Branch("Layer0_errors_fil", &filter_sfiltered.lay0_particles); // Initial state of the filter
  tree_filtered -> Branch("Layer1_errors_fil", &filter_sfiltered.lay1_particles);
  tree_filtered -> Branch("Layer2_errors_fil", &filter_sfiltered.lay2_particles);
  tree_filtered -> Branch("Layer3_errors_fil", &filter_sfiltered.lay3_particles);
  tree_filtered -> Branch("Layer4_errors_fil", &filter_sfiltered.lay4_particles);
  tree_filtered -> Branch("Layer5_errors_fil", &filter_sfiltered.lay5_particles);
  tree_filtered -> Branch("Layer6_errors_fil", &filter_sfiltered.lay6_particles);
  tree_filtered -> Branch("Layer7_errors_fil", &filter_sfiltered.lay7_particles);
  tree_filtered -> Branch("Layer8_errors_fil", &filter_sfiltered.lay8_particles);

  // -- Smoothed states tree
  // Branches
  tree_smoothed -> Branch("Layer0_particles_smo", &filter_smoothed.lay0_particles); // Initial state of the filter
  tree_smoothed -> Branch("Layer1_particles_smo", &filter_smoothed.lay1_particles);
  tree_smoothed -> Branch("Layer2_particles_smo", &filter_smoothed.lay2_particles);
  tree_smoothed -> Branch("Layer3_particles_smo", &filter_smoothed.lay3_particles);
  tree_smoothed -> Branch("Layer4_particles_smo", &filter_smoothed.lay4_particles);
  tree_smoothed -> Branch("Layer5_particles_smo", &filter_smoothed.lay5_particles);
  tree_smoothed -> Branch("Layer6_particles_smo", &filter_smoothed.lay6_particles);
  tree_smoothed -> Branch("Layer7_particles_smo", &filter_smoothed.lay7_particles);
  tree_smoothed -> Branch("Layer8_particles_smo", &filter_smoothed.lay8_particles);

  tree_smoothed -> Branch("Layer0_errors_smo", &filter_ssmoothed.lay0_particles); // Initial state of the filter
  tree_smoothed -> Branch("Layer1_errors_smo", &filter_ssmoothed.lay1_particles);
  tree_smoothed -> Branch("Layer2_errors_smo", &filter_ssmoothed.lay2_particles);
  tree_smoothed -> Branch("Layer3_errors_smo", &filter_ssmoothed.lay3_particles);
  tree_smoothed -> Branch("Layer4_errors_smo", &filter_ssmoothed.lay4_particles);
  tree_smoothed -> Branch("Layer5_errors_smo", &filter_ssmoothed.lay5_particles);
  tree_smoothed -> Branch("Layer6_errors_smo", &filter_ssmoothed.lay6_particles);
  tree_smoothed -> Branch("Layer7_errors_smo", &filter_ssmoothed.lay7_particles);
  tree_smoothed -> Branch("Layer8_errors_smo", &filter_ssmoothed.lay8_particles);


  // --- Detectors
  if (detectors.size() == 0) {
    throw std::invalid_argument("No detector found.");
  }
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Tracker (destructor)
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Tracker::~Tracker() {
  delete tree_filtered;
  delete tree_predicted;
  delete tree_smoothed;

  file_in.Close();
  file_out.Close();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ParticlesFromMeasure
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
vector<vector<Measure>> Tracker::ParticlesFromMeasure(unsigned int event_index){
  // Check on the initialization of the tree
  if (!tree_measures) {
    cerr << "[ERROR] tree_measures is null. Did you forget to set it with SetTreeMeasures()?" << endl;
    return {};
  }

  // Load the e-th event from the measures tree
  if (tree_measures->GetEntry(event_index) <= 0) {
    cerr << "[ERROR] Unable to read entry " << event_index << " from tree_measures." << endl;
    return {};
  }
  
  // Vector of particles (each particle has its hits)
  // TODO: find a way to get this value from the tree or the event
  vector<vector<Measure>> particles(NUMBER_OF_PARTICLES);

  // Layers list to compat the for loop
  const vector<vector<Measure>*> layers = {
    data_measures.lay1_particles, 
    data_measures.lay2_particles,
    data_measures.lay3_particles, 
    data_measures.lay4_particles,
    data_measures.lay5_particles, 
    data_measures.lay6_particles,
    data_measures.lay7_particles, 
    data_measures.lay8_particles
  };

  // Associating hits to a particle based on particleID
  // TODO: pick the number of particles from the vector size
  for (const auto& layer : layers) {
    for (const auto& hit : *layer) {
      if (hit.particleID < static_cast<unsigned int>(NUMBER_OF_PARTICLES)) {
        particles[hit.particleID].push_back(hit);
      } else {
        cout << "[WARNING] Ignoring hit with particleID " << hit.particleID 
                  << " (out of valid range 0.." << NUMBER_OF_PARTICLES - 1 << ")" << endl;
      }
    }
  }

  // Statistics on number of particles reaching n-th layer
  vector<int> count_hits(9, 0);
  for (const auto& track : particles) {
    if (track.size() <= 8) {
        count_hits[track.size()]++;
    } else {
        cerr << "Warning: particle with more than 8 hits (unexpected)" << endl;
    }
  }

  cout << " Summary: Number of hits per particle " << endl;
  for (int i = 0; i <= 8; ++i) {
    cout << " " << i << " hits: " << count_hits[i] << " particles" << endl;
  }
  
  return particles;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Tracking
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool Tracker::Tracking(){
  // -- Creating variables
  vector<vector<Measure>> particles;
  vector<Measurement> measurements;
  
  // Vectors for the Kalman filter particles
  vector<vector<MatrixStateEstimate>> particles_predicted;
  particles_predicted.reserve(particles.size());
  vector<vector<MatrixStateEstimate>> particles_filtered;
  particles_filtered.reserve(particles.size());
  vector<vector<MatrixStateEstimate>> particles_smoothed;
  particles_smoothed.reserve(particles.size());

  // Vectors for the Kalman filter results
  kalmanFilterResult results_filter;
  vector<MatrixStateEstimate> states_predicted;
  vector<MatrixStateEstimate> states_filtered;
  vector<MatrixStateEstimate> states_smoothed;

  // Vectors to fill the tree
  vector<ParticleState> particlestates_predicted;
  vector<ParticleState> particlestates_spredicted;
  vector<ParticleState> particlestates_filtered;
  vector<ParticleState> particlestates_sfiltered;
  vector<ParticleState> particlestates_smoothed;
  vector<ParticleState> particlestates_ssmoothed;

  // -- Loop on events of the tree 
  // TODO: get the number of events from the tree
  for (unsigned int e = 0; e < NUMBER_OF_EVENTS; e++) {
    particles = ParticlesFromMeasure(e);

    // TODO: Insert here the vertexing
    
    // -- Loop on particles
    for (unsigned int p = 0; p < (unsigned int)particles.size(); p++){
      // Kalman filter
      results_filter = kalmanFilter(particles[p], false, false);
      states_predicted = results_filter.predictedStates;
      states_filtered = results_filter.filteredStates;

      cout << " states_filtered detector ID " << states_filtered[2].detectorID << endl;

      // Kalman smoother
      states_smoothed = kalmanSmoother(states_filtered, false);

      // --- Conversion for the tree
      // Filtered
      for (size_t i = 0; i < states_filtered.size(); ++i) {
        // DetectorID
        int detectorID = states_filtered[i].detectorID;
        cout << "states_filtered " << i << "\t detectorID " << detectorID << endl; 

        // Z
        double z = -2.0;
        if(detectorID > 0){
          z = detectors[detectorID-1].getBottmLeftPosition().z();
        } else{
          z = 0.0;
        }

        // States
        ParticleState particle = ParticleState::ParticleStateFromMatrixStateEstimate(states_filtered[i], p, z);
        particlestates_filtered.push_back(particle);

        // Uncertainty
        ParticleState sparticle = ParticleState::sParticleStateFromMatrixStateEstimate(states_filtered[i], p);
        particlestates_sfiltered.push_back(sparticle);
      }

      // Predicted
      for (size_t i = 0; i < states_predicted.size(); ++i) {
        // DetectorID
        int detectorID = states_predicted[i].detectorID;
        cout << "states_predicted " << i << "\t detectorID " << detectorID << endl; 

        // Z
        double z = -2.0;
        if(detectorID > 0){
          z = detectors[detectorID-1].getBottmLeftPosition().z();
        } else{
          z = 0.0;
        }

        // States
        ParticleState particle = ParticleState::ParticleStateFromMatrixStateEstimate(states_predicted[i], p, z);
        particlestates_predicted.push_back(particle);

        // Uncertainty
        ParticleState sparticle = ParticleState::sParticleStateFromMatrixStateEstimate(states_predicted[i], p);
        particlestates_spredicted.push_back(sparticle);
      }

      // Smoothed
      for (size_t i = 0; i < states_smoothed.size(); ++i) {
        // DetectorID
        int detectorID = states_smoothed[i].detectorID;
        cout << "states_smoothed " << i << "\t detectorID " << detectorID << endl; 

        // Z
        double z = -2.0;
        if(detectorID > 0){
          z = detectors[detectorID-1].getBottmLeftPosition().z();
        } else{
          z = 0.0;
        }

        // States
        ParticleState particle = ParticleState::ParticleStateFromMatrixStateEstimate(states_smoothed[i], p, z);
        particlestates_smoothed.push_back(particle);

        // Uncertainty
        ParticleState sparticle = ParticleState::sParticleStateFromMatrixStateEstimate(states_smoothed[i], p);
        particlestates_ssmoothed.push_back(sparticle);
      }

      // Adding the hits to the tree branches
      // Filtered
      for (const auto& pa : particlestates_filtered) {
        switch (pa.detectorID) {
          case 0: filter_filtered.lay0_particles.push_back(pa); break;
          case 1: filter_filtered.lay1_particles.push_back(pa); break;
          case 2: filter_filtered.lay1_particles.push_back(pa); break;
          case 3: filter_filtered.lay1_particles.push_back(pa); break;
          case 4: filter_filtered.lay1_particles.push_back(pa); break;
          case 5: filter_filtered.lay1_particles.push_back(pa); break;
          case 6: filter_filtered.lay1_particles.push_back(pa); break;
          case 7: filter_filtered.lay1_particles.push_back(pa); break;
          case 8: filter_filtered.lay8_particles.push_back(pa); break;
          default: break;
        }
      }

      for (const auto& pa : particlestates_sfiltered) {
        switch (pa.detectorID) {
          case 0: filter_sfiltered.lay0_particles.push_back(pa); break;
          case 1: filter_sfiltered.lay1_particles.push_back(pa); break;
          case 2: filter_sfiltered.lay1_particles.push_back(pa); break;
          case 3: filter_sfiltered.lay1_particles.push_back(pa); break;
          case 4: filter_sfiltered.lay1_particles.push_back(pa); break;
          case 5: filter_sfiltered.lay1_particles.push_back(pa); break;
          case 6: filter_sfiltered.lay1_particles.push_back(pa); break;
          case 7: filter_sfiltered.lay1_particles.push_back(pa); break;
          case 8: filter_sfiltered.lay8_particles.push_back(pa); break;
          default: break;
        }
      }

      // Predicted
      for (const auto& pa : particlestates_predicted) {
        switch (pa.detectorID) {
          case 0: filter_predicted.lay0_particles.push_back(pa); break;
          case 1: filter_predicted.lay1_particles.push_back(pa); break;
          case 2: filter_predicted.lay1_particles.push_back(pa); break;
          case 3: filter_predicted.lay1_particles.push_back(pa); break;
          case 4: filter_predicted.lay1_particles.push_back(pa); break;
          case 5: filter_predicted.lay1_particles.push_back(pa); break;
          case 6: filter_predicted.lay1_particles.push_back(pa); break;
          case 7: filter_predicted.lay1_particles.push_back(pa); break;
          case 8: filter_predicted.lay8_particles.push_back(pa); break;
          default: break;
        }
      }

      for (const auto& pa : particlestates_spredicted) {
        switch (pa.detectorID) {
          case 0: filter_spredicted.lay0_particles.push_back(pa); break;
          case 1: filter_spredicted.lay1_particles.push_back(pa); break;
          case 2: filter_spredicted.lay1_particles.push_back(pa); break;
          case 3: filter_spredicted.lay1_particles.push_back(pa); break;
          case 4: filter_spredicted.lay1_particles.push_back(pa); break;
          case 5: filter_spredicted.lay1_particles.push_back(pa); break;
          case 6: filter_spredicted.lay1_particles.push_back(pa); break;
          case 7: filter_spredicted.lay1_particles.push_back(pa); break;
          case 8: filter_spredicted.lay8_particles.push_back(pa); break;
          default: break;
        }
      }

      // Smoothed
      for (const auto& pa : particlestates_smoothed) {
        switch (pa.detectorID) {
          case 0: filter_smoothed.lay0_particles.push_back(pa); break;
          case 1: filter_smoothed.lay1_particles.push_back(pa); break;
          case 2: filter_smoothed.lay1_particles.push_back(pa); break;
          case 3: filter_smoothed.lay1_particles.push_back(pa); break;
          case 4: filter_smoothed.lay1_particles.push_back(pa); break;
          case 5: filter_smoothed.lay1_particles.push_back(pa); break;
          case 6: filter_smoothed.lay1_particles.push_back(pa); break;
          case 7: filter_smoothed.lay1_particles.push_back(pa); break;
          case 8: filter_smoothed.lay8_particles.push_back(pa); break;
          default: break;
        }
      }

      for (const auto& pa : particlestates_ssmoothed) {
        switch (pa.detectorID) {
          case 0: filter_ssmoothed.lay0_particles.push_back(pa); break;
          case 1: filter_ssmoothed.lay1_particles.push_back(pa); break;
          case 2: filter_ssmoothed.lay1_particles.push_back(pa); break;
          case 3: filter_ssmoothed.lay1_particles.push_back(pa); break;
          case 4: filter_ssmoothed.lay1_particles.push_back(pa); break;
          case 5: filter_ssmoothed.lay1_particles.push_back(pa); break;
          case 6: filter_ssmoothed.lay1_particles.push_back(pa); break;
          case 7: filter_ssmoothed.lay1_particles.push_back(pa); break;
          case 8: filter_ssmoothed.lay8_particles.push_back(pa); break;
          default: break;
        }
      }

      cout << "size di particlestates_ssmoothed " << particlestates_ssmoothed.size() << endl;
      cout << "id di particlestates_ssmoothed " << particlestates_ssmoothed[2].detectorID << endl;
      cout << "size di filter_ssmoothed " << filter_ssmoothed.lay8_particles.size() << endl << endl;

      // Clearing the vectors
      states_filtered.clear();
      states_predicted.clear();
      states_smoothed.clear();
      particlestates_predicted.clear();
      particlestates_spredicted.clear();
      particlestates_filtered.clear();
      particlestates_sfiltered.clear();
      particlestates_smoothed.clear();
      particlestates_ssmoothed.clear();
    }

    // Filling the tree
    tree_filtered -> Fill();
    tree_predicted -> Fill();
    tree_smoothed -> Fill();

    // Clearing the vectors
    particles.clear();
    particles_predicted.clear();
    particles_filtered.clear();
    particles_smoothed.clear();
  }

  tree_filtered -> Write();
  tree_predicted -> Write();
  tree_smoothed -> Write();

  return true;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// estimateNextState
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MatrixStateEstimate Tracker::estimateNextState(const MatrixStateEstimate& preaviousState, double deltaZ) const {
  // Evolution matrix
  double evolutionMatrixData[36] = {
            1., 0., 0., deltaZ, 0.,     0.,
            0., 1., 0., 0.,     deltaZ, 0.,
            0., 0., 1., 0.,     0.,     deltaZ,
            0., 0., 0., 1.,     0.,     0.,
            0., 0., 0., 0.,     1.,     0.,
            0., 0., 0., 0.,     0.,     1.};

  TMatrixD evolutionMatrix(6, 6, evolutionMatrixData);
  TMatrixD estimatedStateValue = TMatrixD(evolutionMatrix, TMatrixD::kMult, preaviousState.value);

  // Evolution of inverse velocity
  double inverseVelocityEvolutionSigma = V_EVOLUTION_SIGMA_KALMAN * pow(estimatedStateValue(3,0), 2);

  // Evolution of uncertainty
  double evolutionUncertaintyData[36] = {
        TIME_EVOLUTION_SIGMA * TIME_EVOLUTION_SIGMA, 0., 0., 0., 0., 0.,
        0., SPACE_EVOLUTION_SIGMA * SPACE_EVOLUTION_SIGMA, 0., 0., 0., 0.,
        0., 0., SPACE_EVOLUTION_SIGMA * SPACE_EVOLUTION_SIGMA, 0., 0., 0.,
        0., 0., 0., inverseVelocityEvolutionSigma * inverseVelocityEvolutionSigma, 0., 0.,
        0., 0., 0., 0., DIRECTION_EVOLUTION_SIGMA * DIRECTION_EVOLUTION_SIGMA, 0.,
        0., 0., 0., 0., 0., DIRECTION_EVOLUTION_SIGMA * DIRECTION_EVOLUTION_SIGMA};

  TMatrixD evolutionUncertainty(6, 6, evolutionUncertaintyData);
  TMatrixD estimatedStateError = TMatrixD(evolutionMatrix, TMatrixD::kMult, TMatrixD(preaviousState.uncertainty, TMatrixD::kMultTranspose, evolutionMatrix));
  estimatedStateError += evolutionUncertainty;

  return MatrixStateEstimate{estimatedStateValue, estimatedStateError};
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// initializeFilterRealTime
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Tracker::initializeFilterRealTime(const vector<Measure> &measures, vector<MatrixStateEstimate> &predictedStates, vector<MatrixStateEstimate> &filteredStates) const {
  // Predicted state
  double predictedData[6] = {measures[0].position.T(), measures[0].position.X(), measures[0].position.Y(), 1. / LIGHT_SPEED, 0., 0.};
  TMatrixD stateValue(6, 1, predictedData);

  // Measure uncertainty
  TMatrixD measureError = consideredDetectors[0].getMeasureUncertainty();

  double firstSdata[36] = {
    measureError(0, 0), 0., 0., 0., 0., 0.,
    0., measureError(1, 1), 0., 0., 0., 0.,
    0., 0., measureError(2, 2), 0., 0., 0.,
    0., 0., 0., bigVInv, 0., 0.,
    0., 0., 0., 0., bigDirection, 0.,
    0., 0., 0., 0., 0., bigDirection};

  TMatrixD stateError(6, 6, firstSdata);

  // Predicted and filtered states
  predictedStates.push_back(MatrixStateEstimate{initialStateValue, initialStateError});
  filteredStates.push_back(MatrixStateEstimate{stateValue, stateError});
  
  if (measures.size() == 1) return;

  // State
  const double deltaZ = consideredDetectors[1].getBottmLeftPosition().Z() - consideredDetectors[0].getBottmLeftPosition().Z();
  const double t = measures[1].position.T();
  const double x = measures[1].position.X();
  const double y = measures[1].position.Y();

  // Variations
  TMatrixD preaviousStateValue = TMatrixD(filteredStates[1].value);
  const double deltaT = t - preaviousStateValue(0, 0);
  const double deltaX = x - preaviousStateValue(1, 0);
  const double deltaY = y - preaviousStateValue(2, 0);

  double data[6] = {t, x, y, deltaT / deltaZ, deltaX / deltaZ, deltaY / deltaZ};
  stateValue.SetMatrixArray(data);

  // Uncertainties
  measureError = consideredDetectors[1].getMeasureUncertainty();
  TMatrixD preaviousStateError = TMatrixD(filteredStates[1].uncertainty);
  const double sDeltaT2 = measureError(0, 0) + preaviousStateError(0, 0);
  const double sDeltaX2 = measureError(1, 1) + preaviousStateError(1, 1);
  const double sDeltaY2 = measureError(2, 2) + preaviousStateError(2, 2);

  double secondSdata[36] = {
    measureError(0, 0), 0., 0., 0., 0., 0.,
    0., measureError(1, 1), 0., 0., 0., 0.,
    0., 0., measureError(2, 2), 0., 0., 0.,
    0., 0., 0., sDeltaT2 / (deltaZ * deltaZ), 0., 0.,
    0., 0., 0., 0., sDeltaX2 / (deltaZ * deltaZ), 0.,
    0., 0., 0., 0., 0., sDeltaY2 / (deltaZ * deltaZ)};

  stateError.SetMatrixArray(secondSdata);
  predictedStates.push_back(MatrixStateEstimate{initialStateValue, initialStateError});
  filteredStates.push_back(MatrixStateEstimate{stateValue, stateError});
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// initializeFilter
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Tracker::initializeFilter(const vector<Measure> &measures, vector<MatrixStateEstimate> &predictedStates, vector<MatrixStateEstimate> &filteredStates) const {
  if (measures.size() == 1) {
    initializeFilterRealTime(measures, predictedStates, filteredStates);
    return;
  }

  // Considered states
  const double t = measures[0].position.T();
  const double x = measures[0].position.X();
  const double y = measures[0].position.Y();
  const double nextT = measures[1].position.T();
  const double nextX = measures[1].position.X();
  const double nextY = measures[1].position.Y();

  // Variations
  const double deltaT = nextT - t;
  const double deltaX = nextX - x;
  const double deltaY = nextY - y;
  const double deltaZ = consideredDetectors[1].getBottmLeftPosition().Z() - consideredDetectors[0].getBottmLeftPosition().Z();

  // State
  double data[6] = {t, x, y, deltaT / deltaZ, deltaX / deltaZ, deltaY / deltaZ};
  TMatrixD stateValue(6, 1, data);

  // Uncertainties
  TMatrixD measureError = consideredDetectors[0].getMeasureUncertainty();
  TMatrixD nextMeasureError = consideredDetectors[1].getMeasureUncertainty();
  const double sDeltaT2 = measureError(0, 0) + nextMeasureError(0, 0);
  const double sDeltaX2 = measureError(1, 1) + nextMeasureError(1, 1);
  const double sDeltaY2 = measureError(2, 2) + nextMeasureError(2, 2);

  double sdata[36] = {measureError(0, 0),           0., 0., 0., 0., 0., 0.,
                      measureError(1, 1),           0., 0., 0., 0., 0., 0.,
                      measureError(2, 2),           0., 0., 0., 0., 0., 0.,
                      sDeltaT2 / (deltaZ * deltaZ), 0., 0., 0., 0., 0., 0.,
                      sDeltaX2 / (deltaZ * deltaZ), 0., 0., 0., 0., 0., 0.,
                      sDeltaY2 / (deltaZ * deltaZ)};
  TMatrixD stateError(6, 6, sdata);

  predictedStates.push_back(MatrixStateEstimate{initialStateValue, initialStateError});
  filteredStates.push_back(MatrixStateEstimate{stateValue, stateError});
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// kalmanFilter
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//kalmanFilterResult Tracker::kalmanFilter(const vector<Measurement> &measures, bool logging, bool realTime) const {
kalmanFilterResult Tracker::kalmanFilter(const vector<Measure> &measures, bool logging, bool realTime) const {
  // Logging
  if (logging) cout << "KALMAN FILTER LOGS" << endl;

  // Variables initialization
  vector<MatrixStateEstimate> filteredStates;
  vector<MatrixStateEstimate> predictedStates;

  // State at the particle gun
  predictedStates.push_back(initialState);
  filteredStates.push_back(initialState);

  int firstMeasureIndex = realTime ? 2 : 1;
  if (realTime)
    initializeFilterRealTime(measures, predictedStates, filteredStates);
  else 
    initializeFilter(measures, predictedStates, filteredStates);

  // Initializing the first state
  for (int i = firstMeasureIndex; i < (int)measures.size(); i++) {
    // Measure
    double measureData[3] = {measures[i].position.T(), measures[i].position.X(), measures[i].position.Y()};
    TMatrixD measure(3, 1, measureData);
    TMatrixD measureError = consideredDetectors[i].getMeasureUncertainty();

    // Previous state
    TMatrixD preaviousStateValue = TMatrixD(filteredStates[i].value);
    TMatrixD preaviousStateError = TMatrixD(filteredStates[i].uncertainty);

    const double deltaZ = consideredDetectors[i].getBottmLeftPosition().Z() - consideredDetectors[i - 1].getBottmLeftPosition().Z();

    double projectiondata[18] = {1., 0., 0., 0., 0., 0., 0., 1., 0.,
                                 0., 0., 0., 0., 0., 1., 0., 0., 0.};
    TMatrixD projectionMatrix(3, 6, projectiondata);

    // Estimate next state
    MatrixStateEstimate estimatedNextState = estimateNextState(filteredStates[i], deltaZ);
    TMatrixD estimatedStateValue = TMatrixD(estimatedNextState.value);
    TMatrixD estimatedStateError = TMatrixD(estimatedNextState.uncertainty);

    // Residual
    TMatrixD residual = TMatrixD(measure, TMatrixD::kMinus, TMatrixD(projectionMatrix, TMatrixD::kMult, estimatedStateValue));

    // Kalman Gain
    TMatrixD kalmanGainDenominator = TMatrixD(projectionMatrix, TMatrixD::kMult, TMatrixD(estimatedStateError, TMatrixD::kMultTranspose, projectionMatrix));

    kalmanGainDenominator += measureError;
    kalmanGainDenominator.SetTol(DETERMINANT_TOLERANCE);
    kalmanGainDenominator.Invert();

    TMatrixD kalmanGain = TMatrixD(TMatrixD(estimatedStateError, TMatrixD::kMultTranspose, projectionMatrix), TMatrixD::kMult, kalmanGainDenominator);
    
    // Filtered state
    TMatrixD filteredStateValue = TMatrixD(kalmanGain, TMatrixD::kMult, residual);

    // Printouts for logging
    if (logging) {
      cout << "Residual: " << endl;
      Utils::printMatrix(residual);
      cout << endl;

      cout << "Preavious State Error" << endl;
      Utils::printMatrix(preaviousStateError);
      cout<<endl;
      
      cout << "Estimated State Error" << endl;
      Utils::printMatrix(estimatedStateError);
      cout<< endl;
      
      cout << "Kalman Gain" << endl;
      Utils::printMatrix(kalmanGain);
      cout << endl << endl << endl;
    }

    // Update filtered state
    filteredStateValue += estimatedStateValue;
    TMatrixD filteredStateError = TMatrixD(kalmanGain, TMatrixD::kMult, TMatrixD(projectionMatrix, TMatrixD::kMult, estimatedStateError));
    filteredStateError = TMatrixD(estimatedStateError, TMatrixD::kMinus, filteredStateError);

    // Filling the vectors to be returned
    predictedStates.push_back(MatrixStateEstimate{estimatedStateValue, estimatedStateError, measures[i].detectorID});
    filteredStates.push_back(MatrixStateEstimate{filteredStateValue, filteredStateError, measures[i].detectorID});
  }

  return kalmanFilterResult{predictedStates, filteredStates};
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// kalmanSmoother
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
vector<MatrixStateEstimate> Tracker::kalmanSmoother(const vector<MatrixStateEstimate> &filteredStates, bool logging) const {
  if (logging) {
    cout << "KALMAN SMOOTHER LOGS" << endl;
  }
  
  // Smoothed state vector
  vector<MatrixStateEstimate> smoothedStates;
  smoothedStates.push_back(filteredStates.back());

  // Evolution matrix
  TMatrixD evolutionMatrix(6,6);

  // Initializing the first state
  for (int i = (int)filteredStates.size() - 2; i > -1; i--) {
    TMatrixD smoothedNextStateValue = TMatrixD(smoothedStates.back().value);
    TMatrixD smoothedNextStateError = TMatrixD(smoothedStates.back().uncertainty);

    // NOTE: This indexes are like this because filteredStates has an element corresponding to the initial state (i.e. at z=0)
    const double deltaZ = i != 0
                            ? consideredDetectors[i].getBottmLeftPosition().Z() - consideredDetectors[i - 1].getBottmLeftPosition().Z()
                            : consideredDetectors[i].getBottmLeftPosition().Z();

    double evolutiondata[36] = {
            1., 0., 0., deltaZ, 0., 0.,
            0., 1., 0., 0., deltaZ, 0.,
            0., 0., 1., 0., 0., deltaZ,
            0., 0., 0., 1., 0., 0.,
            0., 0., 0., 0., 1., 0.,
            0., 0., 0., 0., 0., 1.};

    evolutionMatrix.SetMatrixArray(evolutiondata);

    // Estimation of next state
    MatrixStateEstimate estimatedNextState = estimateNextState(filteredStates[i], deltaZ);
    TMatrixD estimatedNextStateValue = TMatrixD(estimatedNextState.value);
    TMatrixD estimatedNextStateError = TMatrixD(estimatedNextState.uncertainty);
    TMatrixD estimatedNextStateErrorInverted = TMatrixD(estimatedNextStateError);
    estimatedNextStateErrorInverted.SetTol(DETERMINANT_TOLERANCE);

    // Printouts for logging
    if (logging) {
      cout << "Evolution Matrix" << endl;
      Utils::printMatrix(evolutionMatrix);
      cout << endl;

      cout << "Firtered state" << endl;
      Utils::printMatrix(filteredStates[i].uncertainty);
      cout << endl;
      
      cout << "Estimated next state error" << endl;
      Utils::printMatrix(estimatedNextStateErrorInverted);
      cout << endl << endl;
    }

    estimatedNextStateErrorInverted.Invert();

    // Smoother Gain
    TMatrixD smootherGain = TMatrixD(filteredStates[i].uncertainty, TMatrixD::kMultTranspose, evolutionMatrix);

    // Printouts for logging
    if (logging) {
      cout << "Gain first part" << endl;
      Utils::printMatrix(smootherGain);
      cout << endl;
    }

    smootherGain = TMatrixD(smootherGain, TMatrixD::kMult, estimatedNextStateErrorInverted);

    // Residual
    TMatrixD residualValue = TMatrixD(smoothedNextStateValue, TMatrixD::kMinus, estimatedNextStateValue);

    // Smoothed state
    TMatrixD smoothedStateValue = TMatrixD(smootherGain, TMatrixD::kMult, residualValue);
    smoothedStateValue += filteredStates[i].value;

    // Uncertainties
    TMatrixD residualError = TMatrixD(smoothedNextStateError, TMatrixD::kMinus,estimatedNextStateError);
    TMatrixD smoothedStateError = TMatrixD(residualError, TMatrixD::kMultTranspose, smootherGain);
    smoothedStateError = TMatrixD(smootherGain, TMatrixD::kMult, smoothedStateError);
    smoothedStateError += filteredStates[i].uncertainty;

    // Printouts for logging
    if (logging) {
      cout << "Estimated next state error inverted" << endl;
      Utils::printMatrix(estimatedNextStateErrorInverted);
      cout << endl;

      cout << "Smoother gain" << endl;
      Utils::printMatrix(smootherGain);
      cout << endl;

      cout << "Residual error" << endl;
      Utils::printMatrix(residualError);
      cout << endl;

      cout << "Residual value" << endl;
      Utils::printMatrix(residualValue);
      cout << endl << endl;
    }

    smoothedStates.push_back(MatrixStateEstimate{smoothedStateValue, smoothedStateError});
  }

  std::reverse(smoothedStates.begin(), smoothedStates.end());
  return smoothedStates;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// computeChi2s
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Chi2Variables Tracker::computeChi2s(const vector<ParticleState> &expectedStates, const vector<MatrixStateEstimate> &obtainedStates, bool logging, bool skipFirst) const {
  // Variables initialization
  double tChi2 = 0;
  double xChi2 = 0;
  double yChi2 = 0;
  double vChi2 = 0;
  double xzChi2 = 0;
  double yzChi2 = 0;

  // Initial value for the computation
  int initialIndex = (skipFirst) ? 1 : 0;

  // NOTE: The vectors are traversed backwords because sometimes the first elements are not computed
  for (int i = initialIndex; i < (int)obtainedStates.size(); i++) {
      tChi2 += pow((expectedStates[i].position.T() - obtainedStates[i].value(0,0))/sqrt(obtainedStates[i].uncertainty(0,0)), 2);
      xChi2 += pow((expectedStates[i].position.X() - obtainedStates[i].value(1,0))/sqrt(obtainedStates[i].uncertainty(1,1)), 2);
      yChi2 += pow((expectedStates[i].position.Y() - obtainedStates[i].value(2,0))/sqrt(obtainedStates[i].uncertainty(2,2)), 2);
      vChi2 += pow(((1./expectedStates[i].velocity.Z()) - obtainedStates[i].value(3,0))/sqrt(obtainedStates[i].uncertainty(3,3)), 2);
      xzChi2 += pow(((expectedStates[i].velocity.X()/expectedStates[i].velocity.Z()) - obtainedStates[i].value(4,0))/sqrt(obtainedStates[i].uncertainty(4,4)), 2);
      yzChi2 += pow(((expectedStates[i].velocity.Y()/expectedStates[i].velocity.Z()) - obtainedStates[i].value(5,0))/sqrt(obtainedStates[i].uncertainty(5,5)), 2);
  }

  // Printouts for logging
  if (logging) {
    int dof = (skipFirst) ? obtainedStates.size() - 7 : obtainedStates.size() - 6;

    cout <<"t =     " << tChi2 <<  "  | dof = " << dof << "  | pvalue = " <<TMath::Prob(tChi2, dof) << endl;
    cout <<"x =     " << xChi2 <<  "  | dof = " << dof << "  | pvalue = " <<TMath::Prob(xChi2, dof) << endl;
    cout <<"y =     " << yChi2 <<  "  | dof = " << dof << "  | pvalue = " <<TMath::Prob(yChi2, dof) << endl;
    cout <<"1/v_z = " << vChi2 <<  "  | dof = " << dof << "  | pvalue = " <<TMath::Prob(vChi2, dof) << endl;
    cout <<"xz =    " << xzChi2 << "  | dof = " << dof << "  | pvalue = " <<TMath::Prob(xzChi2, dof) << endl;
    cout <<"yz =    " << yzChi2 << "  | dof = " << dof << "  | pvalue = " <<TMath::Prob(yzChi2, dof) << endl;
  }

  return Chi2Variables{tChi2, xChi2, yChi2, vChi2, xzChi2, yzChi2};
}
