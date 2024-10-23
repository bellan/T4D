#pragma once

#include "Detector.hpp"
#include "MeasuresAndStates.hpp"

#include <TMatrixD.h>
#include <vector>

struct kalmanFilterResult {
  std::vector<MatrixStateEstimate> predictedStates;
  std::vector<MatrixStateEstimate> filteredStates;
};

struct Chi2Variables {
  double tChi2, xChi2, yChi2, vChi2, xzChi2, yzChi2;
};

class Tracker {
public:
  Tracker(){};
  Tracker(const std::vector<Detector> &detectors) : allDetectors(detectors), consideredDetectors(detectors) {}

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
  MatrixStateEstimate
  estimateNextState(const MatrixStateEstimate &preaviousState,
                    double deltaZ) const;

  /**
   * Apply the Kalman filter
   *
   * @param measures the vector containing the measures
   * @param logging whether or not to show logs to stdout
   * @param realTime whether or not to initialize the state as if the kalman filter was executed in real time
   * @return a kalmanFilterResult object containing predicted states and filtered states
   */
  kalmanFilterResult kalmanFilter(const std::vector<Measurement> &measures,
                                  bool logging = false,
                                  bool realTime = false) const;

  /**
   * Apply the Kalman smoother
   *
   * @param filteredStates the vector containing the states obtained from the kalman filter
   * @param logging whether or not to show logs to stdout
   * @return a vector containing the smoothed states
   */
  std::vector<MatrixStateEstimate>
  kalmanSmoother(const std::vector<MatrixStateEstimate> &filteredStates,
                 bool looging = false) const;

  /**
   * Compute the chi squared between two set of data
   *
   * @param expectedStates the vector of expected values for the states
   * @param obtainedStates the vector of obtained values for the states
   */
  Chi2Variables
  computeChi2s(const std::vector<ParticleState> &expectedStates,
               const std::vector<MatrixStateEstimate> &obtainedStates,
               bool logging = false, bool skipFirst = false) const;

private:
  std::vector<Detector> allDetectors;
  std::vector<Detector> consideredDetectors;

  void initializeFilterRealTime(
      const std::vector<Measurement> &measures,
      std::vector<MatrixStateEstimate> &predictedStates,
      std::vector<MatrixStateEstimate> &filteredStates) const;
  void initializeFilter(
      const std::vector<Measurement> &measures,
      std::vector<MatrixStateEstimate> &predictedStates,
      std::vector<MatrixStateEstimate> &filteredStates) const;
};
