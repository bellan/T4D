#pragma once

// Header files needed
#include <string>
#include <TFile.h>
#include <TMatrixD.h>
#include <TTree.h>
#include <vector>

// Custom classes
#include "Detector.hpp"
#include "Simulation.hpp"
#include "Structs.hpp"
#include "ParticleState.hpp"
#include "MatrixEstimate.hpp"
#include "Measure.hpp"
#include "StructMeasures.hpp"

// Namespaces
using namespace std;



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// kalmanFilterResults - struct
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
struct kalmanFilterResult {
  vector<MatrixStateEstimate> predictedStates;
  vector<MatrixStateEstimate> filteredStates;
};



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Chi2Variables - struct
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
struct Chi2Variables {
  double tChi2, xChi2, yChi2, vChi2, xzChi2, yzChi2;
};



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Tracker - class
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class Tracker {
public:
  // --- Constructors
  Tracker(){};
  Tracker(const vector<Detector> &detectors) : allDetectors(detectors), consideredDetectors(detectors) {}
  Tracker(const vector<Detector>& detectors, string& path_fin, string& path_fout);

  // --- Destructors
  virtual ~Tracker();

  // --- Public member functions
  // New functions
  vector<vector<Measure>> ParticlesFromMeasure(unsigned int event_index);
  bool Tracking();


  // Original functions
  void ignoreDetector(int detectorIndex) { consideredDetectors.erase(consideredDetectors.begin() + detectorIndex); }
  void resetDetectors() { consideredDetectors = allDetectors; }

  /**
   * Estimate the next state of the particle after a distance deltaZ from a state preaviousState.
   *
   * This function uses the distance deltaZ to compute the evolution matrix to apply to the
   * preavious state. Then, an evolution error is applied to the uncertainty.
   *
   * @param preaviousState the state of the particle at the preavious measure.
   * @param deltaZ the distance covered by the particle.
   * @return the estimated new state of the particle.
   */
  MatrixStateEstimate estimateNextState(const MatrixStateEstimate &preaviousState, double deltaZ) const;

  /**
   * Apply the Kalman filter
   *
   * @param measures the vector containing the measures
   * @param logging whether or not to show logs to stdout
   * @param realTime whether or not to initialize the state as if the kalman filter was executed in real time
   * @return a kalmanFilterResult object containing predicted states and filtered states
   */
  kalmanFilterResult kalmanFilter(const vector<Measure> &measures, bool logging = false, bool realTime = false) const;

  /**
   * Apply the Kalman smoother
   *
   * @param filteredStates the vector containing the states obtained from the kalman filter
   * @param logging whether or not to show logs to stdout
   * @return a vector containing the smoothed states
   */
  vector<MatrixStateEstimate> kalmanSmoother(const vector<MatrixStateEstimate> &filteredStates, bool looging = false) const;

  /**
   * Compute the chi squared between two set of data
   *
   * @param expectedStates the vector of expected values for the states
   * @param obtainedStates the vector of obtained values for the states
   */
  Chi2Variables computeChi2s(const vector<ParticleState> &expectedStates, const vector<MatrixStateEstimate> &obtainedStates, bool logging = false, bool skipFirst = false) const;

private:
  // New data members  
  TFile file_in;
  TFile file_out;

  TTree* tree_measures = nullptr;
  TTree* tree_predicted = nullptr;
  TTree* tree_filtered = nullptr;
  TTree* tree_smoothed = nullptr;

  MeasuresPointer data_measures;
  Data filter_filtered;
  Data filter_sfiltered;
  Data filter_predicted;
  Data filter_spredicted;
  Data filter_smoothed;
  Data filter_ssmoothed;

  vector<Detector> detectors;

  // Original data members TO BE REMOVED after code separation
  vector<Detector> allDetectors;
  vector<Detector> consideredDetectors;

  void initializeFilterRealTime(const vector<Measure> &measures, vector<MatrixStateEstimate> &predictedStates, vector<MatrixStateEstimate> &filteredStates) const;
  void initializeFilter(const vector<Measure> &measures, vector<MatrixStateEstimate> &predictedStates, vector<MatrixStateEstimate> &filteredStates) const;
};
