#pragma once

#include "Detector.hpp"
#include "SetupFactory.hpp"
#include <TMatrixD.h>
#include <vector>

class Tracker {
public:
    Tracker() {};
    Tracker(ExperimentalSetup experimentalSetup): experimentalSetup(experimentalSetup) {}

    ExperimentalSetup experimentalSetup;

    std::vector<std::vector<Measurement>> importMeasurements();
    std::vector<State> fromMeasuresToStates(std::vector<Measurement> measurements);
    std::vector<State> kalmanFilter(std::vector<State> unfilteredStates);
    std::vector<State> kalmanSmoother(std::vector<State> filteredStates);
};
