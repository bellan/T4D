// Header files needed
#include <TLorentzVector.h>
#include <TVector3.h>
#include <cmath>
#include <vector>

// Custom classes
#include "ParticleGun.hpp"
#include "Detector.hpp"
#include "Particle.hpp"
#include "PhysicalParameters.hpp"
#include "RandomGenerator.hpp"



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ParticleGun (constructor) - from TVector3 and double
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ParticleGun::ParticleGun(TVector3 position, double timeOfEmission)
    : position{position}, timeOfEmission{timeOfEmission}, maxColatitude(M_PI / 2.) {}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ParticleGun (constructor) - from TVector3, detector and double
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ParticleGun::ParticleGun(TVector3 position, const std::vector<Detector> &detectors, double timeOfEmission)
    : position(position), timeOfEmission(timeOfEmission) {
  
  // Initial maximum theta value
  double thetaMax = M_PI;

  // For all detectors, definition of geometrical quantities
  for (const Detector &detector : detectors) {
    // Dimensions
    const TVector3 bottomLeft = detector.getBottmLeftPosition();
    const double width = detector.getWidth();
    const double height = detector.getHeight();

    // Detector's vertices
    const TVector3 bottomRight = bottomLeft + TVector3({width, 0, 0});
    const TVector3 topLeft = bottomLeft + TVector3({0, height, 0});
    const TVector3 topRight = bottomLeft + TVector3({width, height, 0});

    // Distances between particle gun and detector's vertices
    const TVector3 vBL = bottomLeft - position;
    const TVector3 vBR = bottomRight - position;
    const TVector3 vTL = topLeft - position;
    const TVector3 vTR = topRight - position;

    // Theta values between particle gun and detector's vertices
    const double thetaBL = vBL.Angle({0, 0, 1});
    const double thetaBR = vBR.Angle({0, 0, 1});
    const double thetaTL = vTL.Angle({0, 0, 1});
    const double thetaTR = vTR.Angle({0, 0, 1});

    // Correction of the maximum theta value
    // NOTE: the 0.9 is accounting for approximation in the sine calculation
    thetaMax = std::min({thetaMax, thetaBL, thetaBR, thetaTL, thetaTR}) * 0.9;
  }

  maxColatitude = thetaMax;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// generateParticle
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// TODO: Change to a more accurate handling of the approximation
Particle ParticleGun::generateParticle() {
  // Getting the random generator instance
  RandomGenerator &randomGenerator = RandomGenerator::getInstance();

  // Generation of particle's direction
  const double phy = randomGenerator.generateLongitude(0., 2. * M_PI);
  const double theta = randomGenerator.generateColatitude(0., maxColatitude);

  // Generation of particle's velocity
  const double vx = sin(theta) * cos(phy);
  const double vy = sin(theta) * sin(phy);
  const double vz = cos(theta);
  const double speed = randomGenerator.generateUniform(MIN_BETA, MAX_BETA) * LIGHT_SPEED;
  const TVector3 velocity(speed * TVector3{vx, vy, vz});

  // Generation of particle's mas and charge
  const double mass = randomGenerator.generateUniform(MIN_PARTICLE_MASS, MAX_PARTICLE_MASS);
  const double charge = FOUNDAMENTAL_CHARGE;

  // Generation of the particle
  const Particle newParticle({position, timeOfEmission}, velocity, mass, charge);

  return newParticle;
}
