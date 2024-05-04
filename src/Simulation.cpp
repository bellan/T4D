#include "Simulation.hpp"

#include <TFile.h>
#include <TLorentzVector.h>
#include <iostream>
#include <stdexcept>

#include "Detector.hpp"
#include "Particle.hpp"
#include "ParticleGun.hpp"
#include "SetupFactory.hpp"

/**
 * The default constructor TODO sobstitude with a factory that creates the experiment settings
 */
Simulation::Simulation():
    detectors(),
    dataFile(TFile::Open("../data/prova.root","RECREATE")),
    tBuffer(0),
    xBuffer(0),
    yBuffer(0),
    idBuffer(1),
    dataTree("DataTree", "DataTree")
{

    dataTree.Branch("t", &tBuffer);
    dataTree.Branch("x", &xBuffer);
    dataTree.Branch("y", &yBuffer);
    dataTree.Branch("id", &idBuffer);

    SetupFactory factory{};
    const ExperimentalSetup experiment = factory.generateExperiment();
    detectors = experiment.detectors;
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
 * The destructor
 *
 * Saves the tree in the file before closing.
 */
Simulation::~Simulation() {
    dataFile->WriteObject(&dataTree, "DataTree");
    dataFile->Close();
}

/**
 * The main simulation function
 *
 * TODO determine what actually does
 */
void Simulation::simulate(int particlesNumber) {
    for (int i=0; i<particlesNumber; i++) {
        Particle particle = particleGun.generateParticle();
        std::cout << " - " << particle.getPositions()[0].T() << " " << particle.getPositions()[0].X() << " " << particle.getPositions()[0].Y() << " " << particle.getPositions()[0].Z() << " " <<
             " - " << particle.getMomentum().E() << " " << particle.getMomentum().X() << " " << particle.getMomentum().Y() << " " << particle.getMomentum().Z() << " " << std::endl;

        for (auto detector : detectors) {
            auto position = particle.zSpaceEvolve(detector.getBottmLeftPosition().z());

            std::optional<Measurement> measure = detector.measure(position);
            if (measure)
                saveData(measure.value());
        }
    }
}

/**
 * Save the data
 *
 * Saves the data to the specified file.
 *
 * @param measure the measure to be saved.
 * @return true if everything succeded, false otherwise.
 */
bool Simulation::saveData(Measurement measure) {
    std::cout<<measure.detectorID<<" " << measure.t << " " <<measure.x<<" " <<measure.y << std::endl;
    tBuffer = measure.t;
    xBuffer = measure.x;
    yBuffer = measure.y;
    idBuffer = measure.detectorID;
    dataTree.Fill();

    return true;
}
