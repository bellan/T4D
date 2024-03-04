#pragma once

#include "Momentum.hpp"
#include "Position.hpp"

class Particle {
public:
    Particle(Coordinates::Momentum momentum=0., Coordinates::Position initialPosition=0., double charge=0.):
        momentum(momentum),
        position(initialPosition),
        charge(charge) {}

    void evolve(double timeStamp);

    Coordinates::Position getPosition() { return position; }

private:
    Coordinates::Momentum momentum;
    Coordinates::Position position;

    double charge;
};
