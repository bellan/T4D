#include "ParticleGun.hpp"

#include "Detector.hpp"
#include "Particle.hpp"

#include <TLorentzVector.h>
#include <TRandom1.h>
#include <TVector3.h>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <iostream>
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
    double thetaMax = M_PI;
    for (auto &detector : detectors) {
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

        const double thetaBL = vBL.Angle({0,0,1});
        const double thetaBR = vBR.Angle({0,0,1});
        const double thetaTL = vTL.Angle({0,0,1});
        const double thetaTR = vTR.Angle({0,0,1});

        thetaMax = std::min(thetaMax, std::min(thetaBL, std::min(thetaBR, std::min(thetaTL, thetaTR)))); //TODO consider changing to max (change also initialization)
    }
    maxColatitude = thetaMax;
    std::cout<<thetaMax<<std::endl;
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
    const double phy = randomGenerator.Uniform(0.,2*M_PI);
    const double uniform_theta = randomGenerator.Uniform(0.,1.);
    const double theta = 2.*asin(sqrt(uniform_theta*(1-cos(maxColatitude))/2.));
    const double vx = sin(theta)*cos(phy);
    const double vy = sin(theta)*sin(phy);
    const double vz = cos(theta);
    const double mass = randomGenerator.Uniform(0.,50.);    //TODO set range and units;
    const double kineticEnergy = randomGenerator.Uniform(0.,50.);
    const double charge = randomGenerator.Uniform(-50.,50.);

    const double momentumModulus = std::sqrt(2*mass*kineticEnergy + kineticEnergy*kineticEnergy/(LIGHT_SPEED_IS*LIGHT_SPEED_IS));

    const TLorentzVector momentum(momentumModulus*TVector3{vx,vy,vz},mass*LIGHT_SPEED_IS);

    const Particle newParticle({position, timeCounter}, momentum, charge);

    timeCounter += randomGenerator.Uniform(0.,50.);
    
    return newParticle;
}
