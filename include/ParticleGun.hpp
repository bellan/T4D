#pragma once

#include "Particle.hpp"

class ParticleGun {
public:
    Particle generateParticle() const;

private:
    double zposition;
};
