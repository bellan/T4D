#include "Simulation.hpp"

#include <Rtypes.h>
#include <TFile.h>
#include <TLorentzVector.h>
#include <TMatrixDfwd.h>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "Detector.hpp"
#include "Particle.hpp"
#include "ParticleGun.hpp"
#include "SetupFactory.hpp"
#include "Tracker.hpp"

/**
 * The default constructor TODO sobstitude with a factory that creates the experiment settings
 */
Simulation::Simulation():
    detectors(),
    dataFile()
{
    SetupFactory factory{};
    const SimulationSetup experiment = factory.generateExperiment();
    detectors = experiment.experimentalSetup.detectors;
    tracker = Tracker(experiment.experimentalSetup);
    particleGun.setMaxColatitude(experiment.particleGun.getMaxColatitude());
    particleGun.setPosition(experiment.particleGun.getPosition());

    if (detectors.size()==0) {
        throw std::invalid_argument("No detector");
    }

    double minLen = detectors[0].getWidth();
    for (auto detector : detectors) {
        if (detector.getWidth() < minLen)
            minLen  = detector.getWidth();
    }

    minTimeInterval = minLen/3e8;
}

/**
 * The main simulation function
 *
 * TODO: determine what actually does
 */
void Simulation::runSimulation(int particlesNumber) {
    std::vector<ParticleState> statiGenerati = generateStatesObjects(particlesNumber);
    auto misureGenerate = fromStatesToMeasures(statiGenerati);
    dataFile.SaveMultipleMeasures(misureGenerate);

    auto misure = dataFile.readMeasures();
    auto misureParticelle = separateMeasuresInParticles(misure);
    for (auto particella : misureParticelle) {
        std::cout<<"PARTICELLA"<<std::endl;
        for (auto misura : particella)
            std::cout<<"Id: "<<misura.detectorID<<"      Misura: "<<misura.t<<" "<<misura.x<<" "<<misura.y<<std::endl;
    }

    std::cout<<"\n\n\n\n\n\n\n"<<std::endl;
    int particleCounter = 1;
    for (auto misureParticellaSingola : misureParticelle) {
        std::cout<<"\n\nParticella "<<particleCounter<<std::endl;
        auto statiDaMisure = tracker.fromMeasuresToStates(misureParticellaSingola);
        std::vector<State> statiFiltrati = tracker.kalmanFilter(statiDaMisure);
        for (int i = 0; i < statiDaMisure.size(); i++) {
            std::cout<<"Atteso: "<<statiGenerati[i].t<<" "<<statiGenerati[i].x<<std::endl;
            std::cout<<"Ottenuto: "<<statiFiltrati[i].value(0,0)<<" "<<statiFiltrati[i].value(0,1)<<std::endl;
        }
        particleCounter++;
    }


}

/**
 * Generate a set of particles and the corresponding measurements
 *
 * @param particleNumber the number of particles to be generated
 * @return a vector containing the measurements generated
 */
// TODO: Remove printing
std::vector<Measurement> Simulation::generateMeasures(int particlesNumber) {
    std::vector<Measurement> measureVector;
    for (int i=0; i<particlesNumber; i++) {
        Particle particle = particleGun.generateParticle();
        std::cout << " - " << particle.getPositions()[0].T() << " " << particle.getPositions()[0].X() << " " << particle.getPositions()[0].Y() << " " << particle.getPositions()[0].Z() << " " <<
             " - " << particle.getMomentum().E() << " " << particle.getMomentum().X() << " " << particle.getMomentum().Y() << " " << particle.getMomentum().Z() << " " << std::endl;

        for (auto detector : detectors) {
            auto position = particle.zSpaceEvolve(detector.getBottmLeftPosition().z());

            std::optional<Measurement> measure = detector.measure(position);
            if (measure)
                measureVector.push_back(measure.value());
        }
    }

    return measureVector;
}

// TODO: documentation or delete
std::vector<ParticleState> Simulation::generateStatesObjects(int particlesNumber) {
    std::vector<ParticleState> stateVector;
    for (int i=0; i<particlesNumber; i++) {
        Particle particle = particleGun.generateParticle();
        std::cout << " - " << particle.getPositions()[0].T() << " " << particle.getPositions()[0].X() << " " << particle.getPositions()[0].Y() << " " << particle.getPositions()[0].Z() << " " <<
             " - " << particle.getMomentum().E() << " " << particle.getMomentum().X() << " " << particle.getMomentum().Y() << " " << particle.getMomentum().Z() << " " << std::endl;

        for (auto detector : detectors) {
            auto position = particle.zSpaceEvolve(detector.getBottmLeftPosition().z());

            std::optional<Measurement> measure = detector.measure(position);
            if (measure)
                stateVector.push_back(ParticleState{measure->detectorID ,measure->t, measure->x, measure->y, detector.getBottmLeftPosition().z(), particle.getMomentum()});
        }
    }
    return stateVector;
}

std::vector<Measurement> Simulation::fromStatesToMeasures(std::vector<ParticleState> states) {
    std::vector<Measurement> measureVector;
    for (ParticleState state : states)
        measureVector.push_back(Measurement{state.t, state.x, state.y, state.detectorId});

    return measureVector;
}


/**
 * Divide the measurement Vector into a vector of vectors containing measures from a single particle for each vector.
 *
 * It first orders the measures based on the time they occour. Then devides them based on the detector id:
 * if the detector of a measure is lower than that of the preavious measure, this is registered as a new particle.
 * 
 * NOTE: This algorithm assumes that before a particle is shot the preavious one has already passed through all the detectors
 * TODO: Implement the abose (increasing the spees should be enough) and uncomment the line in the code.
 * Othewise change the description.
 *
 * @param allMeasures the vector containing the complete sequence of measurements.
 * @param (out)(optional) particlesNumber the number of the particles
 *
 * @return a vector of vector each one representing a single particle
 */
std::vector<std::vector<Measurement>> Simulation::separateMeasuresInParticles(std::vector<Measurement> allMeasures) {
    if (allMeasures.empty()) throw std::invalid_argument("No measures given");

    // std::sort(allMeasures.begin(), allMeasures.end(), [](Measurement a, Measurement b){return a.t < b.t;});

    std::vector<std::vector<Measurement>> singleParticleMeasuresVectors;

    for (Measurement measure : allMeasures) {
        if (singleParticleMeasuresVectors.empty()) {
            singleParticleMeasuresVectors.push_back(std::vector<Measurement>());
        } else if (measure.detectorID < singleParticleMeasuresVectors.back().back().detectorID) {
            singleParticleMeasuresVectors.push_back(std::vector<Measurement>());
        }
        singleParticleMeasuresVectors.back().push_back(measure);
    }

    return singleParticleMeasuresVectors;
}
