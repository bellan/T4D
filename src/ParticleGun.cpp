#include "ParticleGun.hpp"

#include "Detector.hpp"
#include "Particle.hpp"

#include <vector>

ParticleGun::ParticleGun(TVector3 position, std::vector<Detector> detector) {

}

Particle ParticleGun::generateParticle() const {
    
    return Particle(TLorentzVector{}, TLorentzVector{});
}
