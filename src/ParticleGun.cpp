#include "ParticleGun.hpp"

#include "Detector.hpp"
#include "Particle.hpp"
#include "PhysicalParameters.hpp"
#include "RandomGenerator.hpp"

#include <TLorentzVector.h>
#include <TVector3.h>
#include <cmath>
#include <vector>

ParticleGun::ParticleGun(TVector3 position, double timeOfEmission)
    : position{position}, timeOfEmission{timeOfEmission},
      maxColatitude(M_PI / 2.) {}

ParticleGun::ParticleGun(TVector3 position,
                         const std::vector<Detector> &detectors,
                         double timeOfEmission)
    : position(position), timeOfEmission(timeOfEmission) {
  double thetaMax = M_PI;
  for (const Detector &detector : detectors) {
    const TVector3 bottomLeft = detector.getBottmLeftPosition();
    const double width = detector.getWidth();
    const double height = detector.getHeight();
    const TVector3 bottomRight = bottomLeft + TVector3({width, 0, 0});
    const TVector3 topLeft = bottomLeft + TVector3({0, height, 0});
    const TVector3 topRight = bottomLeft + TVector3({width, height, 0});
    const TVector3 vBL = bottomLeft - position;
    const TVector3 vBR = bottomRight - position;
    const TVector3 vTL = topLeft - position;
    const TVector3 vTR = topRight - position;

    const double thetaBL = vBL.Angle({0, 0, 1});
    const double thetaBR = vBR.Angle({0, 0, 1});
    const double thetaTL = vTL.Angle({0, 0, 1});
    const double thetaTR = vTR.Angle({0, 0, 1});

    // NOTE: the 0.9 is accounting for approximation in the sine calculation
    thetaMax = std::min({thetaMax, thetaBL, thetaBR, thetaTL, thetaTR}) * 0.9;
  }
  maxColatitude = thetaMax;
}

// TODO: Change to a more accurate handling of the approximation
Particle ParticleGun::generateParticle() {
  RandomGenerator &randomGenerator = RandomGenerator::getInstance();
  const double phy = randomGenerator.generateLongitude(0., 2. * M_PI);
  const double theta = randomGenerator.generateColatitude(0., maxColatitude);
  const double vx = sin(theta) * cos(phy);
  const double vy = sin(theta) * sin(phy);
  const double vz = cos(theta);
  const double mass =
      randomGenerator.generateUniform(MIN_PARTICLE_MASS, MAX_PARTICLE_MASS);
  const double charge = FOUNDAMENTAL_CHARGE;
  const double speed =
      randomGenerator.generateUniform(MIN_BETA, MAX_BETA) * LIGHT_SPEED;

  const TVector3 velocity(speed * TVector3{vx, vy, vz});

  const Particle newParticle({position, timeOfEmission}, velocity, mass,
                             charge);

  return newParticle;
}
