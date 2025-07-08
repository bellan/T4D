#pragma once

// Header files needed
#include <TLorentzVector.h>
#include <TMatrixD.h>
#include <TVector3.h>
#include <TObject.h>

// Custom classes
#include "MatrixEstimate.hpp"


class ParticleState : public TObject {
  public:
    // Constructors
    ParticleState(TLorentzVector position = {0,0,0,0}, TVector3 velocity = {0,0,0}, int detectorID = -99, unsigned int particleID = 4294967295) 
      : position{position}, velocity{velocity}, detectorID{detectorID}, particleID{particleID}
      {};

    // Destructor
    virtual ~ParticleState();

    // Public member functions
    static ParticleState ParticleStateFromMatrixStateEstimate(MatrixStateEstimate& m, unsigned int particleID = 4294967295, double z = -3.0);
    static ParticleState sParticleStateFromMatrixStateEstimate(MatrixStateEstimate& m, unsigned int particleID = 4294967295);

    // Data members
    TLorentzVector position;
    TVector3 velocity;
    int detectorID;
    unsigned int particleID;
    
  ClassDef(ParticleState, 1)
};
