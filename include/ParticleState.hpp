#pragma once

// Header files needed
#include <TLorentzVector.h>
#include <TMatrixD.h>
#include <TVector3.h>
#include <TObject.h>

/**
 * The measuremnt struct.
 * Destructor for the ParticleState class.
 *
 * It contains the data produced by the detector.
 * This destructor is responsible for cleaning up any resources allocated by the class.
 * Since the class inherits from TObject, it is important to call the base class destructor.
 *
 * @return void
 */

class ParticleState : public TObject {
  public:
    // Constructors
    ParticleState(TLorentzVector position = {0,0,0,0}, TVector3 velocity = {0,0,0}, int detectorID = -99, unsigned int particleID = 4294967295) 
      : position{position}, velocity{velocity}, detectorID{detectorID}, particleID{particleID}
      {};

    // Destructor
    virtual ~ParticleState();

    // Data members
    TLorentzVector position;
    TVector3 velocity;
    int detectorID;
    unsigned int particleID;
    
  ClassDef(ParticleState, 1)
};
