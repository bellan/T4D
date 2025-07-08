// Headers needed
#include <TLorentzVector.h>

// Custom classes
#include "ParticleState.hpp"
#include "MatrixEstimate.hpp"

// ROOT things
ClassImp(ParticleState);



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ParticleState (destructor)
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ParticleState::~ParticleState() {}


ParticleState ParticleState::ParticleStateFromMatrixStateEstimate(MatrixStateEstimate& m, unsigned int particleID, double z){
  // DetectorID
  int detectorID = m.detectorID;

  // LorentzVector
  TLorentzVector pos;
  pos.SetXYZT(m.value(1,0), m.value(2,0), z, m.value(0,0));

  // Velocity
  TVector3 vel(m.value(3,0), m.value(4,0), m.value(5,0));

  return ParticleState(pos, vel, detectorID, particleID);
}


ParticleState ParticleState::sParticleStateFromMatrixStateEstimate(MatrixStateEstimate& m, unsigned int particleID){
  // DetectorID
  int det = m.detectorID;

  // LorentzVector
  TLorentzVector pos;
  pos.SetXYZT(m.uncertainty(1,1), m.uncertainty(2,2), 0.0, m.uncertainty(0,0));

  // Velocity
  TVector3 vel(m.uncertainty(3,3), m.uncertainty(4,4), m.uncertainty(5,5));

  return ParticleState(pos, vel, det, particleID);
}