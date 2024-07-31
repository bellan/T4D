#include "Particle.hpp"
#include "MeasuresAndStates.hpp"
#include "PhisicalParameters.hpp"
#include "RandomGenerator.hpp"

#include <TLorentzVector.h>
#include <TVector3.h>
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
Particle::Particle(const TLorentzVector initialPosition, const TVector3 velocity, const double mass, const double charge):
states{},
mass{mass},
charge{charge} {
    states.push_back(ParticleState{initialPosition, velocity});
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
    const ParticleState lastState = states.back();
    const TLorentzVector lastPosition = lastState.position;
    const TVector3 lastVelocity = lastState.velocity;

    const double deltaZ = finalZ - lastState.position.Z();
    if (deltaZ <= 0)
        throw std::invalid_argument("Invalid final Z position. It is before the last position.");

    const double lastVZ = lastState.velocity.z();
    const double lastXZ = lastState.velocity.x() / lastVZ;
    const double lastYZ = lastState.velocity.y() / lastVZ;

    const double deltaT = deltaZ/lastVZ;

    RandomGenerator &randomGenerator = RandomGenerator::getInstance();
    const double variationT = randomGenerator.generateGaussian(0., TIME_EVOLUTION_SIGMA);
    const double variationX = randomGenerator.generateGaussian(0., SPACE_EVOLUTION_SIGMA);
    const double variationY = randomGenerator.generateGaussian(0., SPACE_EVOLUTION_SIGMA);
    const double variationVZ = - fabs(randomGenerator.generateGaussian(0., VELOCITY_EVOLUTION_SIGMA));
    const double variationXZ = randomGenerator.generateGaussian(0., DIRECTION_EVOLUTION_SIGMA);
    const double variationYZ = randomGenerator.generateGaussian(0., DIRECTION_EVOLUTION_SIGMA);

    const TLorentzVector newPosition{
        lastPosition.X() + lastXZ * deltaZ + variationX,
        lastPosition.Y() + lastYZ * deltaZ + variationY,
        finalZ,
        lastPosition.T() + deltaT + variationT
    };

    double newVZ = lastVZ + variationVZ;
    std::cout<<variationVZ<<" "<<newVZ<<std::endl;
    const TVector3 newVelocity{
        (lastXZ + variationXZ) * newVZ,
        (lastYZ + variationYZ) * newVZ,
        newVZ
    };

    states.push_back(ParticleState{newPosition, newVelocity});
    return newPosition;
}
