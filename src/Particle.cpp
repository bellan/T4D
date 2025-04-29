// Header files needed
#include <TLorentzVector.h>
#include <TVector3.h>
#include <optional>
#include <stdexcept>

// Custom classes
#include "Particle.hpp"
#include "PhysicalParameters.hpp"
#include "RandomGenerator.hpp"


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Particle (constructor)
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Particle::Particle(const TLorentzVector initialPosition, const TVector3 initialVelocity, const double mass, const double charge)
    : initialState{initialPosition, initialVelocity, -1, Particle::INVALID_ID}, mass{mass}, charge{charge}, ID{Particle::INVALID_ID} {}


Particle::Particle(const TLorentzVector initialPosition, const TVector3 initialVelocity, const double mass, const double charge, id_t ID)
    : initialState{initialPosition, initialVelocity, -1, ID}, mass{mass}, charge{charge}, ID{ID} {}


Particle::Particle(const ParticleState& state)
    : initialState(state), mass(0.0), charge(0.0), ID(state.particleID) {}



Particle::~Particle() {}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// zSpaceEvolve
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//ParticleState Particle::zSpaceEvolve(ParticleState preaviousState, double finalZ, bool multipleScattering, std::optional<int> detectorId) const {
ParticleState Particle::zSpaceEvolve(ParticleState preaviousState, double finalZ, bool multipleScattering, int detectorId) const {
  // Starting position and velocity
  const TLorentzVector lastPosition = preaviousState.position;
  const TVector3 lastVelocity = preaviousState.velocity;
  const double lastVZ = preaviousState.velocity.z();
  const double lastXZ = preaviousState.velocity.x() / lastVZ;
  const double lastYZ = preaviousState.velocity.y() / lastVZ;

  // Check if it is the last detector or it the particle is going backwards
  const double deltaZ = finalZ - preaviousState.position.Z();
  if (deltaZ <= 0){
    throw std::invalid_argument("Invalid final Z position. It is before the last position.");
  }

  // Time evolution
  const double deltaT = deltaZ / lastVZ;

  // Activation of multiple scattering if necessary
  if (!multipleScattering) {
    // Evolution of position and velocity according to the motion equation
    const TLorentzVector newPosition{lastPosition.X() + lastXZ * deltaZ, lastPosition.Y() + lastYZ * deltaZ, finalZ, lastPosition.T() + deltaT};
    const TVector3 newVelocity{lastXZ * lastVZ, lastYZ * lastVZ, lastVZ};

    return ParticleState{newPosition, newVelocity, detectorId, preaviousState.particleID};
  }
  else{
    // Getting the random generator instance
    RandomGenerator &randomGenerator = RandomGenerator::getInstance();

    // Random variations
    const double variationT = fabs(randomGenerator.generateGaussian(0., TIME_EVOLUTION_SIGMA));
    const double variationX = randomGenerator.generateGaussian(0., SPACE_EVOLUTION_SIGMA);
    const double variationY = randomGenerator.generateGaussian(0., SPACE_EVOLUTION_SIGMA);
    const double variationVZ = -fabs(randomGenerator.generateGaussian(0., VELOCITY_EVOLUTION_SIGMA));
    const double variationXZ = randomGenerator.generateGaussian(0., DIRECTION_EVOLUTION_SIGMA);
    const double variationYZ = randomGenerator.generateGaussian(0., DIRECTION_EVOLUTION_SIGMA);

    // Evolution of position according to the motion equation
    const TLorentzVector newPosition{lastPosition.X() + lastXZ * deltaZ + variationX, lastPosition.Y() + lastYZ * deltaZ + variationY, finalZ, lastPosition.T() + deltaT + variationT};

    // Evolution of velocity according to the motion equation
    double newVZ = lastVZ + variationVZ;
    const TVector3 newVelocity{(lastXZ + variationXZ) * newVZ, (lastYZ + variationYZ) * newVZ, newVZ};

    return ParticleState{newPosition, newVelocity, detectorId, preaviousState.particleID};
  }
}
