#pragma once

// Header files needed
#include <TLorentzVector.h>
#include <TMatrixD.h>
#include <TVector3.h>
#include <TObject.h>
#include <optional>
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
    ParticleState(TLorentzVector position = {0,0,0,0}, TVector3 velocity = {0,0,0}, int detectorID = {-99}) : position{position}, velocity{velocity}, detectorID{detectorID} 
      {};

    // Destructor
    virtual ~ParticleState();

    // Data members
    TLorentzVector position;
    TVector3 velocity;
    std::optional<int> detectorID = std::nullopt;
    //int detectorID = -1;
    
  //ClassDef(ParticleState, 1)
};
