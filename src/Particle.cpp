#include "Particle.hpp"

void Particle::evolve(double timeStamp) {
    const double xVelocity = momentum.px/momentum.energy*3e8;
    const double yVelocity = momentum.py/momentum.energy*3e8;
    const double zVelocity = momentum.pz/momentum.energy*3e8;
    position.t += timeStamp;
    position.x += timeStamp*xVelocity;
    position.y += timeStamp*yVelocity;
    position.z += timeStamp*zVelocity;
}
