#pragma once

// Header files needed
#include <TLorentzVector.h>
#include <TObject.h>

// Custom classes
#include "ParticleState.hpp"


// Class
class Measure : public TObject {
  public:
    // Constructors
    Measure() = default;
    Measure(TLorentzVector position, int detectorID, unsigned int particleID)
      : position(position), detectorID(detectorID), particleID(particleID) {}
    Measure(ParticleState state)
      : position(state.position), detectorID(state.detectorID), particleID(state.particleID) {}
  
    // Destructor
    virtual ~Measure();

    // Data members
    TLorentzVector position;
    int detectorID;
    unsigned int particleID;
    
    ClassDef(Measure, 1)
};