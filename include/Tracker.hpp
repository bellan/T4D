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

  MatrixStateEstimate
  estimateNextState(const MatrixStateEstimate &preaviousState,
                    double deltaZ) const;

  kalmanFilterResult kalmanFilter(const std::vector<Measurement> &measures,
                                  bool logging = false,
                                  bool realTime = false) const;

  std::vector<MatrixStateEstimate>
  kalmanSmoother(const std::vector<MatrixStateEstimate> &filteredStates,
                 bool looging = false) const;

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
