#include "Particle.hpp"

#include <TLorentzVector.h>
#include <TMatrixD.h>
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
velocity{velocity},
positions{},
mass{mass},
charge{charge} {
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

    const double vZ = velocity.z();
    const double deltaT = deltaZ/vZ;

    const double vX = velocity.x();
    const double vY = velocity.y();

    const TLorentzVector newPosition{
        lastPosition.X() + vX * deltaT,
        lastPosition.Y() + vY * deltaT,
        finalZ,
        lastPosition.T() + deltaT
    };

    this->positions.push_back(newPosition);
    return newPosition;
}

std::vector<TMatrixD> Particle::getStates() {
    std::vector<TMatrixD> statesVector;
    double speedInverse = 1/velocity.Mag();
    double tanThetaxz = velocity.x() / velocity.z();
    double tanThetayz = velocity.y() / velocity.z();
    for(auto position: positions) {
        double data[6] = {position.T(), position.X(), position.Y(), speedInverse, tanThetaxz, tanThetayz};
        statesVector.push_back(TMatrixD(6,1,data));
    }

    return statesVector;
}
