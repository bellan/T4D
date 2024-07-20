#pragma once

#include <vector>

#include "DataFile.hpp"
#include "Detector.hpp"
#include "ParticleGun.hpp"

class Simulation {
public:
    Simulation();
    void simulate(int particlesNumber);

private:
    double minTimeInterval;
    ParticleGun particleGun;
    std::vector<Detector> detectors;

    DataFile dataFile;
};
