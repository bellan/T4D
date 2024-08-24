#pragma once

#include "Detector.hpp"
#include "MeasuresAndStates.hpp"

#include <TMatrixD.h>
#include <vector>

struct kalmanFilterResult {
  std::vector<MatrixStateEstimate> predictedStates;
  std::vector<MatrixStateEstimate> filteredStates;
};

class Tracker {
public:
  Tracker(){};
  Tracker(const std::vector<Detector> &detectors) : detectors(detectors) {}

  kalmanFilterResult
  kalmanFilter(const std::vector<Measurement> &measures, bool logging=false) const;

  std::vector<MatrixStateEstimate>
  kalmanSmoother(const std::vector<MatrixStateEstimate> &filteredStates, bool looging=false) const;

private:
  std::vector<Detector> detectors;
};
