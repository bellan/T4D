#pragma once

#include <TLorentzVector.h>
#include <vector>

#include "DataFile.hpp"
#include "Detector.hpp"
#include "ParticleGun.hpp"
#include "Tracker.hpp"

struct ParticleState {
    int detectorId;
    double t, x, y, z;
    TLorentzVector momentum;
};

class Simulation {
public:
    Simulation();
    void runSimulation(int particlesNumber);
    std::vector<Measurement> generateMeasures(int particlesNumber);
    std::vector<ParticleState> generateStatesObjects(int particlesNumber);
    std::vector<Measurement> fromStatesToMeasures(std::vector<ParticleState> states);
    std::vector<std::vector<Measurement>> separateMeasuresInParticles(std::vector<Measurement> allMeasures);

private:
    double minTimeInterval;
    ParticleGun particleGun;
    std::vector<Detector> detectors;

    DataFile dataFile;
    Tracker tracker;
};
