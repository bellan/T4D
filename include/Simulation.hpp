#pragma once

#include <TLorentzVector.h>
#include <TMatrixD.h>
#include <TMatrixDfwd.h>
#include <vector>

#include "DataFile.hpp"
#include "Detector.hpp"
#include "ParticleGun.hpp"
#include "Tracker.hpp"

class Simulation {
public:
    Simulation();
    void runSimulation(int particlesNumber);
    std::vector<Measurement> generateMeasures(int particlesNumber);
    std::vector<Particle> generateParticlesAndEvolve(int particlesNumber);
    std::vector<std::vector<Measurement>> separateMeasuresInParticles(std::vector<Measurement> allMeasures);
    void saveDataToCSV(std::vector<std::vector<TMatrixD>> generatedStates, std::vector<std::vector<State>> filteredStates, std::vector<std::vector<State>> smoothedStates, std::vector<std::vector<State>> predictedStates);

private:
    double minTimeInterval;
    ParticleGun particleGun;
    std::vector<Detector> detectors;

    DataFile dataFile;
    Tracker tracker;
};
