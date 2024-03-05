#include "Detector.hpp"
#include "Particle.hpp"
#include "ParticleGun.hpp"
#include <TLorentzVector.h>
#include "Simulation.hpp"

// #include "TFile.h"

Simulation::Simulation():
    particleGun(TVector3{}),
    detectors()
{
    double minLen = detectors[0].getWidth();
    for (auto detector : detectors) {
        if (detector.getWidth() < minLen)
            minLen  = detector.getWidth();
    }

    minTimeInterval = minLen/3e8;

}

void Simulation::simulate(int eventNumber, int maxIterations) const {
    for (int i=0; i<eventNumber; i++) {
        Particle particle = particleGun.generateParticle();
        for (int j=0; j<maxIterations; j++) {
            TLorentzVector position = particle.timeEvolve(minTimeInterval);
            for (auto detector : detectors) {
                std::optional<Measurement> measure = detector.measure(position);
            }
        } 
    }
}
