#include "Particle.hpp"

#include <TLorentzVector.h>
#include <stdexcept>

/**
 * The constructor
 *
 * It instantiate a Particle object with its inital momentum and position.
 * The mass of the particle can be determined from its momentum.
 *
 * @param initialPosition the poisition at which the particle is generated.
 * @param momentum the momentum of the particle at its generation.
 * @param charge the charge of the particle.
 */
Particle::Particle(const TLorentzVector initialPosition, const TLorentzVector momentum, const double charge):
mass{momentum.M()/LIGHT_SPEED_IS},
charge{charge},
momentum{momentum},
positions{} {
    positions.push_back(initialPosition);
}

/**
 * The space evolution function.
 *
 * It evolves the particle to the desired z position.
 * It than adds the new position to the vector.
 *
 * @param finalZ the position in meters.
 *
 * @return the new position after the evolution.
 */
TLorentzVector Particle::zSpaceEvolve(const double finalZ) {
    const TLorentzVector lastPosition = this->positions.back();
    const double deltaZ = finalZ - lastPosition.Z();
    if (deltaZ <= 0)
        throw std::invalid_argument("Invalid final Z position. It is before the last position.");

    const double vZ = momentum.Pz()/momentum.E()*LIGHT_SPEED_IS;
    const double deltaT = deltaZ/vZ;

    const double vX = momentum.Px()/momentum.E()*LIGHT_SPEED_IS;
    const double vY = momentum.Py()/momentum.E()*LIGHT_SPEED_IS;

    const TLorentzVector newPosition{
        lastPosition.X() + vX * deltaT,
        lastPosition.Y() + vY * deltaT,
        finalZ,
        lastPosition.T() + deltaT
    };

    this->positions.push_back(newPosition);
    return newPosition;
}

/**
 * The time evolution function
 *
 * It generates the new position of the particle after timeStamp seconds after the last position.
 * It then adds the new position to the vector.
 *
 * @param timeStep the seconds of evolution after the last position.
 *
 * @return the new position after the evolution.
 */
TLorentzVector Particle::timeEvolve(const double timeStep) {
    if (timeStep<0)
        throw std::invalid_argument("Invalid time-step. Time cannot be negative");

    const TLorentzVector lastPosition = this->positions.back();
    TLorentzVector velocity = LIGHT_SPEED_IS / momentum.E() * momentum;
    velocity.SetT(velocity.T()/LIGHT_SPEED_IS);

    const TLorentzVector newPosition = lastPosition + velocity * timeStep;

    this->positions.push_back(newPosition);
    return newPosition;
}
