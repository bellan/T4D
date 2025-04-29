#pragma once

#include <vector>
#include <TClonesArray.h>
#include "PhysicalParameters.hpp"

#include "Particle.hpp"
#include "ParticleState.hpp"


// --- Data saving struct



// --- Andrea's structs

/**
 * The measuremnt struct.
 *
 * It contains the data produced by the detector.
 */
struct Measurement {
  double t;
  double x;
  double y;
  int detectorID;
};


struct GeneratedData {
  std::vector<std::vector<ParticleState>> allParticlesTheoreticalStates;
  std::vector<std::vector<ParticleState>> allParticlesRealStates;
  std::vector<std::vector<Measurement>> allParticlesMeasures;
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