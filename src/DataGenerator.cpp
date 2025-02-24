// Header files needed
#include <iomanip>
#include <vector>

// Custom classes
#include "DataGenerator.hpp"
#include "PhysicalParameters.hpp"
#include "MeasuresAndStates.hpp"

// Namespaces
using namespace std;



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// generateParticleStates
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
vector<ParticleState> DataGenerator::generateParticleStates(Particle particle, bool multipleScattering) const {
  // Vector of the state of the particle on the particle gun and the detectors
  vector<ParticleState> particleStates;
  particleStates.reserve(simulationSetup.detectors.size() + 1);

  // Add the state of the particle on the particle gun
  particleStates.push_back(particle.getInitialState());

  // For each detector, propagate the particle
  for (const Detector &detector : simulationSetup.detectors) {
    const ParticleState newState = particle.zSpaceEvolve(particleStates.back(), detector.getBottmLeftPosition().z(), multipleScattering, detector.getId());
    particleStates.push_back(newState);
  }

  // Return all the states of the particle
  return particleStates;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// generateParticleMeasures
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
vector<Measurement> DataGenerator::generateParticleMeasures(vector<ParticleState> &particleStates) const {
  // Vector of the measurements of the particle on the detectors
  vector<Measurement> measureVector;
  measureVector.reserve(simulationSetup.detectors.size());

  // For each state in the vector of particle states, simulate the measurement
  for (const ParticleState &state : particleStates) {
    // If the particle is not inside any detector, break the loop
    if (!state.detectorID)
      continue;

    // Simulate the measurement
    std::optional<Measurement> measure = simulationSetup.detectors[state.detectorID.value()].measure(state);

    // If measurement exits the detector, print out
    if (!measure) {
      if (LOGS){
        const int id = state.detectorID.value();
        cout << "Particle went out of detector line at detector " << id + 1 << " (id = " << id << ")" << endl;
      }

      break;
    }

    measureVector.push_back(measure.value());
  }

  // Return all the measurements of the particle
  return measureVector;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// generateAllData
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
GeneratedData DataGenerator::generateAllData(int particlesNumber, bool enableLogging, bool useMultipleScattering) {
  // Vectors to store the states, measurements, and theoretical states of the particles
  vector<vector<ParticleState>> allParticlesTheoreticalStates;
  allParticlesTheoreticalStates.reserve(particlesNumber);
  vector<vector<ParticleState>> allParticlesRealStates;
  allParticlesRealStates.reserve(particlesNumber);
  vector<vector<Measurement>> allParticlesMeasures;
  allParticlesMeasures.reserve(particlesNumber);

  // For each particle, generate and store the data
  for (int i = 0; i < particlesNumber; i++) {
    Particle particle = generateParticle();

    vector<ParticleState> theoreticalParticleStates = generateParticleStates(particle, false);
    vector<ParticleState> realParticleStates = generateParticleStates(particle, useMultipleScattering);
    vector<Measurement> particleMeasures = generateParticleMeasures(realParticleStates);

    allParticlesTheoreticalStates.push_back(theoreticalParticleStates);
    allParticlesRealStates.push_back(realParticleStates);
    allParticlesMeasures.push_back(particleMeasures);
  }

  const GeneratedData results{allParticlesTheoreticalStates, allParticlesRealStates, allParticlesMeasures};
  if (enableLogging)
    logData(results);

  return results;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// logData
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void DataGenerator::logData(const GeneratedData &generatedData) const {
  cout << "GENERATED DATA LOGS" << endl;
  cout << setprecision(5);

  // Vectors of the particles
  const vector<vector<ParticleState>> &allParticlesTheoreticalStates = generatedData.allParticlesTheoreticalStates;
  const vector<vector<ParticleState>> &allParticlesRealStates = generatedData.allParticlesRealStates;
  const vector<vector<Measurement>> &allParticlesMeasures = generatedData.allParticlesMeasures;

  // For each particle, print things
  for (int j = 0; j < (int)allParticlesMeasures.size(); j++) {
    cout << "Particle " << j << "\n";
    cout << "z    |theoretical        |real     |measured\n";
    cout << "z    |t,x,y,1/vz,xz,yz|t,x,y,1/vz,xz,yz|t,x,y,t,x,y\n";

    const vector<Measurement> &measures = allParticlesMeasures[j];
    const vector<ParticleState> &theoStates = allParticlesTheoreticalStates[j];
    const vector<ParticleState> &realStates = allParticlesRealStates[j];

    // Print z 
    cout << theoStates[0].position.Z() << " | ";

    // Print theoretical state
    cout << theoStates[0].position.T() << "," << theoStates[0].position.X() << "," << theoStates[0].position.Y() << "," << 1. / theoStates[0].velocity.Z() << ","
         << theoStates[0].velocity.X() / theoStates[0].velocity.Z() << ","  << theoStates[0].velocity.Y() / theoStates[0].velocity.Z() << "|";

    // Print real state (theoretical + multiple scattering)
    cout << realStates[0].position.T() << "," << realStates[0].position.X()  << "," << realStates[0].position.Y() << "," << 1. / realStates[0].velocity.Z() << ","
         << realStates[0].velocity.X() / realStates[0].velocity.Z() << "," << realStates[0].velocity.Y() / realStates[0].velocity.Z() << endl;

    // Print measurement state
    for (int i = 0; i < (int)measures.size(); i++) {
      Measurement meas = measures[i];
      ParticleState the = theoStates[i + 1];
      ParticleState rea = realStates[i + 1];

      // Print z
      cout << the.position.Z() << " | ";

      // Print theoretical state
      cout << the.position.T() << "," << the.position.X() << "," << the.position.Y() << "," << 1. / the.velocity.Z() << ","
           << the.velocity.X() / the.velocity.Z() << "," << the.velocity.Y() / the.velocity.Z() << "|";

      // Print real state (theoretical + multiple scattering)
      cout << rea.position.T() << "," << rea.position.X() << "," << rea.position.Y() << "," << 1. / rea.velocity.Z() << ","
           << rea.velocity.X() / rea.velocity.Z() << "," << rea.velocity.Y() / rea.velocity.Z() << "|";

      // Print measurement
      cout << meas.t << "," << meas.x << "," << meas.y << endl;
    }
  }
  cout << "\n" << endl;
}
