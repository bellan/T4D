// Header files needed
#include <vector>

// Custom classes
#include "Measure.hpp"
#include "Structs.hpp"

// ROOT things
ClassImp(Measure);



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Measure (destructor)
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Measure::~Measure() {}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MeasurementFromMeasure
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Measurement Measure::MeasurementFromMeasure(Measure& particle){
  Measurement m;
  m.t = particle.position.T();
  m.x = particle.position.X();
  m.y = particle.position.Y();
  m.detectorID = particle.detectorID;
  return m;
}

vector<Measurement> Measure::vMeasurementFromMeasure(vector<Measure>& particles) {
  vector<Measurement> measurements;
  measurements.reserve(particles.size());

  for (auto& particle : particles) {
    measurements.push_back(MeasurementFromMeasure(particle));
  }
 
  return measurements;
}