#pragma once

#include "Detector.hpp"
#include "MeasuresAndStates.hpp"

#include <TMatrixD.h>
#include <vector>

class Tracker {
public:
    Tracker() {};
    Tracker(std::vector<Detector> detectors): detectors(detectors) {}

    std::vector<Detector> detectors;

    std::vector<std::vector<Measurement>> importMeasurements();
    std::vector<MatrixStateEstimate> kalmanFilter(std::vector<Measurement> measures, std::vector<MatrixStateEstimate> &predictions);
    std::vector<MatrixStateEstimate> kalmanSmoother(std::vector<MatrixStateEstimate> filteredStates);
};
