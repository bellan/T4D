#include "ParticleGun.hpp"

#include "Detector.hpp"
#include "Particle.hpp"

#include <TLorentzVector.h>
#include <TRandom1.h>
#include <TVector3.h>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <vector>

/**
 * Default constructor
 *
 * It generates the particle gun assuming that all angles are valid to shoot.
 *
 * @param position the position of the gun.
 */
ParticleGun::ParticleGun(TVector3 position):
position{position},
maxColatitude(M_PI/2.),
randomGenerator(std::time(NULL)),
timeCounter(0.) {}

/**
 * constructor with specified shooting angle
 *
 * It generates the particle gun determining the angles to shoot at.
 *
 * @param position the position of the gun.
 * @param detector a vector of all the detectors used to determine the valid angles.
 */
ParticleGun::ParticleGun(TVector3 position, std::vector<Detector> detectors):
position(position),
randomGenerator(std::time(NULL)),
timeCounter(0.) {
    double thetaMax = 0.;
    for (auto &detector : detectors) {
        const TVector3 v = position - detector.getBottmLeftPosition();
        const double theta = v.Angle({0,0,1});
        thetaMax = std::max(thetaMax, theta);
    }
}

/**
 * Generate a random particle
 *
 * It generates a particle in the position of the gun with a momentum directed to a random angle
 * in the range of valid angles.
 *
 * @return the particle generated
 */
Particle ParticleGun::generateParticle() {
    double x, y, z;
    randomGenerator.Sphere(x, y, z, 1.); //TODO consider the angle range
    const double mass = randomGenerator.Uniform(0.,50.);    //TODO set range and units;
    const double kineticEnergy = randomGenerator.Uniform(0.,50.);
    const double charge = randomGenerator.Uniform(-50.,50.);

    const double momentumModulus = std::sqrt(2*mass*kineticEnergy + kineticEnergy*kineticEnergy/(LIGHT_SPEED_IS*LIGHT_SPEED_IS));

    const TLorentzVector momentum(momentumModulus*TVector3{x,y,z},mass*LIGHT_SPEED_IS);

    const Particle newParticle({position, timeCounter}, momentum, charge);

    timeCounter += randomGenerator.Uniform(0.,50.);
    
    return newParticle;
}
