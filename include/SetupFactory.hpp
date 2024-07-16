#pragma once

#include "Detector.hpp"
#include "ParticleGun.hpp"

struct ExperimentalSetup {
public:
    std::vector<Detector> detectors;
};

struct SimulationSetup {
public:
    ParticleGun particleGun;
    ExperimentalSetup experimentalSetup;
};

class SetupFactory {
public:
    SetupFactory() {}
    SimulationSetup generateExperiment() const;
};
