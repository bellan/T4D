// Header files needed
#include <cmath>
#include <optional>
#include <TLorentzVector.h>
#include <TMatrixD.h>

// Custom classes
#include "Detector.hpp"
#include "Structs.hpp"
#include "Measure.hpp"
#include "ParticleState.hpp"
#include "PhysicalParameters.hpp"
#include "RandomGenerator.hpp"



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Global class counters
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Number of detectors
int Detector::counter = 0;



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Detector (constructor)
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Detector::Detector(double zPosition, double width, double height)
    : id{counter}, width{width}, height{height}, bottomLeftPosition{-width / 2., -height / 2., zPosition} {
  counter++;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Detector (constructor)
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Detector::Detector(double zPosition, double width, double height, int ID)
    : id{ID}, width{width}, height{height}, bottomLeftPosition{-width / 2., -height / 2., zPosition} {
  counter++;
}


Detector::~Detector(){}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Measure - from TLotentzVector
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
std::optional<Measurement> Detector::measure(TLorentzVector particlePosition) const {
  // Measurement of the generated particle to be measured
  const double x = particlePosition.X();
  const double y = particlePosition.Y();
  const double deltaZ = particlePosition.Z() - this->bottomLeftPosition.z();

  // Geometrical constraints for the particle to be in the detector
  const bool xConstrain = x > bottomLeftPosition.x() && x < bottomLeftPosition.x() + width;
  const bool yConstrain = y > bottomLeftPosition.y() && y < bottomLeftPosition.y() + height;
  const bool zConstrain = deltaZ == 0;

  // Gaussian smearing based on detector uncertainty
  RandomGenerator &randomGenerator = RandomGenerator::getInstance();
  double measuredT = randomGenerator.generateGaussian(particlePosition.T(), DETECTOR_TIME_UNCERTAINTY);
  const double measuredX = randomGenerator.generateGaussian(x, DETECTOR_SPACE_UNCERTAINTY);
  const double measuredY = randomGenerator.generateGaussian(y, DETECTOR_SPACE_UNCERTAINTY);

  // Test to see if a time measurement of 0 breaks everything
  if(id == 5){
    measuredT = 0.0;
  }

  // Return the measurement if it satisfies the geometrical constraints
  return (xConstrain && yConstrain && zConstrain) ? std::optional<Measurement>{{measuredT, measuredX, measuredY, id}} : std::nullopt;
}


std::optional<Measure> Detector::measure2(TLorentzVector position, unsigned int particleID) const {
  // Measurement of the generated particle to be measured
  const double x = position.X();
  const double y = position.Y();
  const double z = position.Z();
  const double deltaZ = position.Z() - this->bottomLeftPosition.z();

  // Geometrical constraints for the particle to be in the detector
  const bool xConstrain = x > bottomLeftPosition.x() && x < bottomLeftPosition.x() + width;
  const bool yConstrain = y > bottomLeftPosition.y() && y < bottomLeftPosition.y() + height;
  const bool zConstrain = deltaZ == 0;

  // Gaussian smearing based on detector uncertainty
  RandomGenerator &randomGenerator = RandomGenerator::getInstance();
  double measuredT = randomGenerator.generateGaussian(position.T(), DETECTOR_TIME_UNCERTAINTY);
  const double measuredX = randomGenerator.generateGaussian(x, DETECTOR_SPACE_UNCERTAINTY);
  const double measuredY = randomGenerator.generateGaussian(y, DETECTOR_SPACE_UNCERTAINTY);

  // Test to see if a time measurement of 0 breaks everything
  if(id == 5){
    measuredT = 0.0;
  }

  // Creating the LorentzVector
  TLorentzVector measuredPosition(measuredX, measuredY, z, measuredT);

  // Return the measurement if it satisfies the geometrical constraints
  return (xConstrain && yConstrain && zConstrain) ? std::optional<Measure>{Measure(measuredPosition, id, particleID)} : std::nullopt;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Measure - from TMatrixD
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
std::optional<Measurement> Detector::measure(TMatrixD state) const {
  // Measurement of the generated particle to be measured
  const double t = state(0, 0);
  const double x = state(1, 0);
  const double y = state(2, 0);

  // Building the TLorentzVector
  const TLorentzVector position{x, y, this->bottomLeftPosition.z(), t};

  // Calling the other "measure" function to get the measurement
  return measure(position);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Measure - from ParticleState
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
std::optional<Measurement> Detector::measure(ParticleState state) const {
  // Calling the other "measure" function to get the measurement
  return measure(state.position);
}

std::optional<Measure> Detector::measure2(ParticleState state) const {
  // Calling the other "measure" function to get the measurement
  return measure2(state.position, state.particleID);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// getMeasureUncertainty
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TMatrixD Detector::getMeasureUncertainty() const {
  // Vector with evaluated uncertainties
  double sdata[36] = {
      DETECTOR_TIME_UNCERTAINTY * DETECTOR_TIME_UNCERTAINTY,   0., 0.,
      0., DETECTOR_SPACE_UNCERTAINTY * DETECTOR_SPACE_UNCERTAINTY, 0.,
      0., 0., DETECTOR_SPACE_UNCERTAINTY * DETECTOR_SPACE_UNCERTAINTY};

  // Creating the TMatrixD object
  TMatrixD uncertainty(3, 3, sdata);

  // Returning the uncertainty matrix
  return uncertainty;
}
