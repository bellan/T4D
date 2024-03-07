#include "Simulation.hpp"

#include <TFile.h>
#include <TLorentzVector.h>
#include <ctime>

#include "Detector.hpp"
#include "Particle.hpp"
#include "ParticleGun.hpp"
#include "SetupFactory.hpp"

/**
 * The default constructor TODO sobstitude with a factory that creates the experiment settings
 */
Simulation::Simulation():
    detectors(),
    dataFile(TFile::Open("prova.root","RECREATE")),
    dataTree("T", "Prova")
{
    double minLen = detectors[0].getWidth();
    for (auto detector : detectors) {
        if (detector.getWidth() < minLen)
            minLen  = detector.getWidth();
    }

    minTimeInterval = minLen/3e8;
    const auto timeNow = std::time(NULL);

    dataTree.Branch("measure", &measureBuffer);

    SetupFactory factory{};
    const ExperimentalSetup experiment = factory.generateExperiment();
    detectors = experiment.detectors;
    particleGun.setMaxColatitude(experiment.particleGun.getMaxColatitude());
    particleGun.setPosition(experiment.particleGun.getPosition());

}

/**
 * The destructor
 *
 * Saves the tree in the file before closing.
 */
Simulation::~Simulation() {
    dataFile->WriteObject(&dataTree, "DataTree");
}

/**
 * The main simulation function
 *
 * TODO determine what actually does
 */
void Simulation::simulate(int particlesNumber, int maxIterations) {
    for (int i=0; i<particlesNumber; i++) {
        Particle particle = particleGun.generateParticle();
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
    measureBuffer = measure;
    dataTree.Fill();

    return true;
}
