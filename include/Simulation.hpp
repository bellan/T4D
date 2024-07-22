#pragma once

#include <vector>

#include "DataFile.hpp"
#include "Detector.hpp"
#include "ParticleGun.hpp"

class Simulation {
public:
    Simulation();
    void runSimulation(int particlesNumber);
    std::vector<std::vector<Measurement>> separateMeasuresInParticles(std::vector<Measurement> allMeasures);

private:
    double minTimeInterval;
    ParticleGun particleGun;
    std::vector<Detector> detectors;

    DataFile dataFile;
};
