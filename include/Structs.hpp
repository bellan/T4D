#pragma once

// Header files needed
#include <TClonesArray.h>
#include <vector>

// Custom classes
#include "ParticleState.hpp"

// Namespaces
using namespace std;


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// --- Andrea's structs
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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