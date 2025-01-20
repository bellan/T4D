#include "Particle.hpp"

#include "MeasuresAndStates.hpp"
#include "PhysicalParameters.hpp"
#include "RandomGenerator.hpp"

#include <TLorentzVector.h>
#include <TVector3.h>
#include <optional>
#include <stdexcept>

Particle::Particle(const TLorentzVector initialPosition,
                   const TVector3 initialVelocity, const double mass,
                   const double charge)
    : initialState{initialPosition, initialVelocity}, mass{mass},
      charge{charge} {}

ParticleState Particle::zSpaceEvolve(ParticleState preaviousState,
                                     double finalZ,
                                     bool multipleScattering,
                                     std::optional<int> detectorId) const {
  const TLorentzVector lastPosition = preaviousState.position;
  const TVector3 lastVelocity = preaviousState.velocity;

  const double deltaZ = finalZ - preaviousState.position.Z();
  if (deltaZ <= 0)
    throw std::invalid_argument(
        "Invalid final Z position. It is before the last position.");

  const double lastVZ = preaviousState.velocity.z();
  const double lastXZ = preaviousState.velocity.x() / lastVZ;
  const double lastYZ = preaviousState.velocity.y() / lastVZ;

  const double deltaT = deltaZ / lastVZ;

  if (!multipleScattering) {
    const TLorentzVector newPosition{lastPosition.X() + lastXZ * deltaZ,
                                     lastPosition.Y() + lastYZ * deltaZ, finalZ,
                                     lastPosition.T() + deltaT};

    const TVector3 newVelocity{lastXZ * lastVZ, lastYZ * lastVZ, lastVZ};

    return ParticleState{newPosition, newVelocity, detectorId};
  }

  RandomGenerator &randomGenerator = RandomGenerator::getInstance();
  const double variationT =
      fabs(randomGenerator.generateGaussian(0., TIME_EVOLUTION_SIGMA));
  const double variationX =
      randomGenerator.generateGaussian(0., SPACE_EVOLUTION_SIGMA);
  const double variationY =
      randomGenerator.generateGaussian(0., SPACE_EVOLUTION_SIGMA);
  const double variationVZ =
      -fabs(randomGenerator.generateGaussian(0., VELOCITY_EVOLUTION_SIGMA));
  const double variationXZ =
      randomGenerator.generateGaussian(0., DIRECTION_EVOLUTION_SIGMA);
  const double variationYZ =
      randomGenerator.generateGaussian(0., DIRECTION_EVOLUTION_SIGMA);

  const TLorentzVector newPosition{
      lastPosition.X() + lastXZ * deltaZ + variationX,
      lastPosition.Y() + lastYZ * deltaZ + variationY, finalZ,
      lastPosition.T() + deltaT + variationT};

  double newVZ = lastVZ + variationVZ;
  const TVector3 newVelocity{(lastXZ + variationXZ) * newVZ,
                             (lastYZ + variationYZ) * newVZ, newVZ};

  return ParticleState{newPosition, newVelocity, detectorId};
}
