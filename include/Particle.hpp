#pragma once

#include "Momentum.hpp"
#include "Position.hpp"

class Particle {
public:
    Particle(Coordinates::Momentum momentum=0., Coordinates::Position initialPosition=0., double charge=0.):
        momentum(momentum),
        initialPosition(initialPosition),
        charge(charge) {}

private:
    Coordinates::Momentum momentum;
    Coordinates::Position initialPosition;

    double charge;
}
