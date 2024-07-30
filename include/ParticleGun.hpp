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
    ParticleGun(TVector3 position = TVector3());
    ParticleGun(TVector3 position, std::vector<Detector> detectors);

    void setMaxColatitude(double newValue) { maxColatitude = newValue; }
    void setPosition(TVector3 newPosition) { position = newPosition; }
    double getMaxColatitude() const { return maxColatitude; }
    TVector3 getPosition() const { return position; }
    Particle generateParticle();

private:
    TVector3 position;

    double maxColatitude;

    double timeCounter;
};
