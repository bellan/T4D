#pragma once

#include <memory>
#include <vector>
#include <TFile.h>
#include <TTree.h>

#include "Detector.hpp"
#include "ParticleGun.hpp"

class Simulation {
public:
    Simulation();
    ~Simulation();
    void simulate(int particlesNumber, int maxIterations); 
    bool saveData(Measurement measure);

private:
    double minTimeInterval;
    ParticleGun particleGun;
    std::vector<Detector> detectors;
    
    std::unique_ptr<TFile> dataFile;
    Measurement measureBuffer;

    TTree dataTree;

};
