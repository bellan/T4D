#pragma once

#include <TVector3.h>
#include <numbers>
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
    ParticleGun(TVector3 position): position(position), maxColatitude(std::numbers::pi/2.) {}
    ParticleGun(TVector3 position, std::vector<Detector> detectors);

    Particle generateParticle() const;

private:
    TVector3 position;

    double maxColatitude;
};
