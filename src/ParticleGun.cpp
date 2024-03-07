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

ParticleGun::ParticleGun(TVector3 position):
position{position},
maxColatitude{std::numbers::pi/2.},
randomGenerator(std::time(NULL)),
timeCounter(0.) {}

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

Particle ParticleGun::generateParticle() {
    double x, y, z;
    randomGenerator.Sphere(x, y, z, 1.);
    const double mass = randomGenerator.Uniform(0.,50.);    //TODO set range and units;
    const double kineticEnergy = randomGenerator.Uniform(0.,50.);
    const double charge = randomGenerator.Uniform(-50.,50.);

    const double momentumModulus = std::sqrt(2*mass*kineticEnergy + kineticEnergy*kineticEnergy/(LIGHT_SPEED_IS*LIGHT_SPEED_IS));

    const TLorentzVector momentum(momentumModulus*TVector3{x,y,z},mass*LIGHT_SPEED_IS);

    const Particle newParticle({position, timeCounter}, momentum, charge);

    timeCounter += randomGenerator.Uniform(0.,50.);
    
    return newParticle;
}
