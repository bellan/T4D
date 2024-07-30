#pragma once

#include "Detector.hpp"
#include "ParticleGun.hpp"

#include <vector>

struct SimulationSetup {
public:
    ParticleGun particleGun;
    std::vector<Detector> detectors;
};

class SetupFactory {
public:
    SetupFactory() {}
    SimulationSetup generateExperiment() const;
};
