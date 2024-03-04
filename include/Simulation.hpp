#pragma once

#include <vector>

#include "Detector.hpp"
#include "ParticleGun.hpp"

class Simulation {
public:
    Simulation();
    void simulate(int eventNumber, int maxIterations) const; 

private:
    double minTimeInterval;
    ParticleGun particleGun;
    std::vector<Detector> detectors;
};
