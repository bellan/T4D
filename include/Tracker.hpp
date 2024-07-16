#pragma once

#include "Detector.hpp"
#include "SetupFactory.hpp"
#include <TMatrixDfwd.h>
#include <vector>

struct States {
public:
    std::vector<TMatrixD> values;
    std::vector<TMatrixD> uncertainties;
};

class Tracker {
public:
    std::vector<std::vector<Measurement>> importMeasurements();
    States kalmanFilter(std::vector<Measurement> measurements);
    States kalmanSmoother(States filteredStates);
    ExperimentalSetup experimentalSetup;



};
