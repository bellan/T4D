#include "Detector.hpp"
#include "Particle.hpp"
#include "ParticleGun.hpp"
#include "Position.hpp"
#include "Simulation.hpp"

// #include "TFile.h"

Simulation::Simulation():
    particleGun(),
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
            particle.evolve(minTimeInterval);
            Coordinates::Position position = particle.getPosition();
            for (auto detector : detectors) {
                std::optional<Measurement> measure = detector.measure(position);
            }
        } 
    }
}
