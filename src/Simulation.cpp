#include "Simulation.hpp"

#include <TFile.h>
#include <TLorentzVector.h>
#include <TMatrixD.h>
#include <TMatrixDfwd.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "Detector.hpp"
#include "Particle.hpp"
#include "ParticleGun.hpp"
#include "PhisicalParameters.hpp"
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

    minTimeInterval = MIN_TIME_BETWEEN_PARTICLE;
}

/**
 * The main simulation function
 *
 * TODO: determine what actually does
 */
void Simulation::runSimulation(int particlesNumber) {
    std::vector<Particle> evolvedParticles = generateParticlesAndEvolve(particlesNumber);
    std::cout<<evolvedParticles.size()<<std::endl;
    std::vector<std::vector<TMatrixD>> generatedParticlesStates;
    std::vector<Measurement> generatedMeasures;
    for (Particle particle: evolvedParticles) {
        std::vector<TMatrixD> registeredParticleStates;
        std::vector<TMatrixD> generatedParticleStates = particle.getStates();
        registeredParticleStates.push_back(generatedParticleStates[0]);
        for (int i = 0; i < (int)detectors.size(); i++) {
            std::optional<Measurement> measure = detectors[i].measure(generatedParticleStates[i+1]);
            if (measure) {
                generatedMeasures.push_back(measure.value());
                registeredParticleStates.push_back(generatedParticleStates[i+1]);
            }
        }
        generatedParticlesStates.push_back(registeredParticleStates);
    }
    dataFile.SaveMultipleMeasures(generatedMeasures);

    auto misure = dataFile.readMeasures();
    auto misureParticelle = separateMeasuresInParticles(misure);
    for (auto particella : misureParticelle) {
        std::cout<<"PARTICELLA"<<std::endl;
        for (auto misura : particella)
            std::cout<<"Id: "<<misura.detectorID<<"      Misura: "<<misura.t<<" "<<misura.x<<" "<<misura.y<<std::endl;
    }

    std::vector<std::vector<State>> predictedStates;
    std::vector<std::vector<State>> filteredStates;
    std::vector<std::vector<State>> smoothedStates;
    std::cout<<"\n\n"<<std::endl;
    for (int j = 0; j < (int)generatedParticlesStates.size(); j++) {
        std::cout<<"\n\nParticella "<<j+1<<std::endl;
        auto misureParticellaSingola = misureParticelle[j];
        auto statiParticellaSingola = generatedParticlesStates[j];
        std::vector<State> predizioni;
        std::vector<State> statiFiltrati = tracker.kalmanFilter(misureParticellaSingola, predizioni);
        std::vector<State> statiSmoothed = tracker.kalmanSmoother(statiFiltrati);
        predictedStates.push_back(predizioni);
        filteredStates.push_back(statiFiltrati);
        smoothedStates.push_back(statiSmoothed);
    }

    saveDataToCSV(generatedParticlesStates, filteredStates, smoothedStates, predictedStates);
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
             " - " << particle.getVelocity().Mag() << " " << particle.getVelocity().X() << " " << particle.getVelocity().Y() << " " << particle.getVelocity().Z() << " " << std::endl;

        for (Detector detector : detectors) {
            auto position = particle.zSpaceEvolve(detector.getBottmLeftPosition().z());

            std::optional<Measurement> measure = detector.measure(position);
            if (measure)
                measureVector.push_back(measure.value());
        }
    }

    return measureVector;
}

/**
 * Generate a set of particles and make them evolve
 *
 * @param particleNumber the number of particles to be generated
 * @return a vector containing the particles
 */
std::vector<Particle> Simulation::generateParticlesAndEvolve(int particlesNumber) {
    std::vector<Particle> particlesVector;
    for (int i=0; i<particlesNumber; i++) {
        Particle particle = particleGun.generateParticle();
        for (Detector detector : detectors) {
            particle.zSpaceEvolve(detector.getBottmLeftPosition().z());
        }
        particlesVector.push_back(particle);
    }

    return particlesVector;
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

// TODO: document
void Simulation::saveDataToCSV(std::vector<std::vector<TMatrixD>> generatedStates, std::vector<std::vector<State>> filteredStates, std::vector<std::vector<State>> smoothedStates, std::vector<std::vector<State>> predictedStates) {
    if (filteredStates.size() != generatedStates.size() || filteredStates.size() != smoothedStates.size() || smoothedStates.size() != predictedStates.size()) std::cout<<"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"<<std::endl;
    for (int j = 0; j < (int)generatedStates.size(); j++) {
        std::string filename("../data/Particle ");
        filename += std::to_string(j);
        filename += ".csv";
        std::ofstream csvFile;
        csvFile.open(filename);
        csvFile << "z,generated,,,,,,predicted,,,,,,,,,,,,filtered,,,,,,,,,,,,smoothed,,,,,,,,,,,\n";
        csvFile << "z,t,x,y,speeed,xz,yz,t,st,x,sx,y,sy,speeed,sspeed,xz,sxz,yz,syz,t,st,x,sx,y,sy,speeed,sspeed,xz,sxz,yz,syz,t,st,x,sx,y,sy,speeed,sspeed,xz,sxz,yz,syz\n";
        for (int i = 0; i < (int)generatedStates[j].size(); i++) {
            if (i == 0)
                csvFile << "0.,";
            else
                csvFile << detectors[i-1].getBottmLeftPosition().z() << ",";

            TMatrixD gen = generatedStates[j][i];
            State pre = predictedStates[j][i];
            State fil = filteredStates[j][i];
            State smo = smoothedStates[j][i];

            csvFile << gen(0,0) << ",";
            csvFile << gen(1,0) << ",";
            csvFile << gen(2,0) << ",";
            csvFile << gen(3,0) << ",";
            csvFile << gen(4,0) << ",";
            csvFile << gen(5,0) << ",";

            csvFile << pre.value(0,0) << ",";
            csvFile << sqrt(pre.uncertainty(0,0)) << ",";
            csvFile << pre.value(1,0) << ",";
            csvFile << sqrt(pre.uncertainty(1,1)) << ",";
            csvFile << pre.value(2,0) << ",";
            csvFile << sqrt(pre.uncertainty(2,2)) << ",";
            csvFile << pre.value(3,0) << ",";
            csvFile << sqrt(pre.uncertainty(3,3)) << ",";
            csvFile << pre.value(4,0) << ",";
            csvFile << sqrt(pre.uncertainty(4,4)) << ",";
            csvFile << pre.value(5,0) << ",";
            csvFile << sqrt(pre.uncertainty(5,5)) << ",";

            csvFile << fil.value(0,0) << ",";
            csvFile << sqrt(fil.uncertainty(0,0)) << ",";
            csvFile << fil.value(1,0) << ",";
            csvFile << sqrt(fil.uncertainty(1,1)) << ",";
            csvFile << fil.value(2,0) << ",";
            csvFile << sqrt(fil.uncertainty(2,2)) << ",";
            csvFile << fil.value(3,0) << ",";
            csvFile << sqrt(fil.uncertainty(3,3)) << ",";
            csvFile << fil.value(4,0) << ",";
            csvFile << sqrt(fil.uncertainty(4,4)) << ",";
            csvFile << fil.value(5,0) << ",";
            csvFile << sqrt(fil.uncertainty(5,5)) << ",";

            csvFile << smo.value(0,0) << ",";
            csvFile << sqrt(smo.uncertainty(0,0)) << ",";
            csvFile << smo.value(1,0) << ",";
            csvFile << sqrt(smo.uncertainty(1,1)) << ",";
            csvFile << smo.value(2,0) << ",";
            csvFile << sqrt(smo.uncertainty(2,2)) << ",";
            csvFile << smo.value(3,0) << ",";
            csvFile << sqrt(smo.uncertainty(3,3)) << ",";
            csvFile << smo.value(4,0) << ",";
            csvFile << sqrt(smo.uncertainty(4,4)) << ",";
            csvFile << smo.value(5,0) << ",";
            csvFile << sqrt(smo.uncertainty(5,5)) << "\n";
        }
        csvFile.close();
    }
}
