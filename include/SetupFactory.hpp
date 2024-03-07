#pragma once

#include "Detector.hpp"
#include "ParticleGun.hpp"

struct ExperimentalSetup {
public:
    ParticleGun particleGun;
    std::vector<Detector> detectors;
};

class SetupFactory {
public:
    SetupFactory() {}
    ExperimentalSetup generateExperiment() const;
};
