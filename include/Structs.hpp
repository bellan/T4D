#pragma once

// Header files needed
#include <TClonesArray.h>
#include <vector>

// Custom classes
#include "Measure.hpp"
#include "Particle.hpp"
#include "ParticleState.hpp"
#include "PhysicalParameters.hpp"

// Namespaces
using namespace std;


// --- Data saving struct
// Struct for the measures
struct Measures{
  // Hits on each detector
  vector<Measure> lay1_particles;
  vector<Measure> lay2_particles;
  vector<Measure> lay3_particles;
  vector<Measure> lay4_particles;
  vector<Measure> lay5_particles;
  vector<Measure> lay6_particles;
  vector<Measure> lay7_particles;
  vector<Measure> lay8_particles;
};



// --- Andrea's structs

/**
 * The measuremnt struct. TO BE REMOVED after code separation
 *
 * It contains the data produced by the detector.
 */
struct Measurement {
  double t;
  double x;
  double y;
  int detectorID;
};


// TO BE REMOVED after code separation
struct GeneratedData {
  vector<vector<ParticleState>> allParticlesTheoreticalStates;
  vector<vector<ParticleState>> allParticlesRealStates;
  vector<vector<Measurement>> allParticlesMeasures;
};

/**
 * The matrix rapresentation of the estimated state.
 *
 * It contains the value and the uncertainty of the estimate.
 */
struct MatrixStateEstimate {
public:
  TMatrixD value;
  TMatrixD uncertainty;
  int detectorID = -2;
};