#include "SetupFactory.hpp"
#include "Detector.hpp"
#include "ParticleGun.hpp"
#include <vector>

ExperimentalSetup SetupFactory::generateExperiment() const {
    std::vector<Detector> detectors = {};
    for (int i = 1; i < 13; i++) {
        detectors.push_back(Detector(i* 5., 50, 50));
    }

    const ParticleGun gun({0,0,0}, detectors);
    return {gun, detectors};
}
