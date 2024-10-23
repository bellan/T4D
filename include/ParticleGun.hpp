#pragma once

#include <TRandom1.h>
#include <TVector3.h>
#include <vector>

#include "Detector.hpp"
#include "Particle.hpp"

/**
 * The particle generator.
 *
 * It generates the particle to be shot.
 */
class ParticleGun {
public:
  /**
   * Default constructor
   *
   * It generates the particle gun assuming that all angles are valid to shoot.
   *
   * @param position the position of the gun.
   */
  ParticleGun(TVector3 position = TVector3(), double timeOfEmission = 0.);

  /**
   * constructor with specified shooting angle
   *
   * It generates the particle gun determining the angles to shoot at.
   *
   * @param position the position of the gun.
   * @param detector a vector of all the detectors used to determine the valid
   * angles.
   */
  ParticleGun(TVector3 position, const std::vector<Detector> &detectors, double timeOfEmission = 0.);

  void setMaxColatitude(double newValue) { maxColatitude = newValue; }
  void setPosition(TVector3 newPosition) { position = newPosition; }
  double getMaxColatitude() const { return maxColatitude; }
  TVector3 getPosition() const { return position; }

  /**
   * Generate a random particle
   *
   * It generates a particle in the position of the gun with a momentum directed
   * to a random angle in the range of valid angles.
   *
   * @return the particle generated
   */
  Particle generateParticle();

private:
  TVector3 position;
  double timeOfEmission;

  double maxColatitude;
};
