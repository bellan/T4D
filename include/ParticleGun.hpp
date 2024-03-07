#pragma once

#include <TVector3.h>
#include <TRandom1.h>
#include <vector>

#include "Detector.hpp"
#include "Particle.hpp"

/**
 * The particle generator.
 *
 * It generates the particle to be shot.
 */
class ParticleGun {
public:
    ParticleGun(TVector3 position);
    ParticleGun(TVector3 position, std::vector<Detector> detectors);

    Particle generateParticle();

private:
    TVector3 position;

    double maxColatitude;

    TRandom1 randomGenerator;
    double timeCounter;
};
