// Header files needed
#include <cmath>
#include <optional>
#include <TLorentzVector.h>
#include <TMatrixD.h>

// Custom classes
#include "Detector.hpp"
#include "MeasuresAndStates.hpp"
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

  if(id == 5){
    measuredT = 0.0;
  }

  // Return the measurement if it satisfies the geometrical constraints
  return (xConstrain && yConstrain && zConstrain) ? std::optional<Measurement>{{measuredT, measuredX, measuredY, id}} : std::nullopt;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Measure - from TMatrixD
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
std::optional<Measurement> Detector::measure(TMatrixD particleState) const {
  // Measurement of the generated particle to be measured
  const double t = particleState(0, 0);
  const double x = particleState(1, 0);
  const double y = particleState(2, 0);

  // Building the TLorentzVector
  const TLorentzVector particlePosition{x, y, this->bottomLeftPosition.z(), t};

  // Calling the other "measure" function to get the measurement
  return measure(particlePosition);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Measure - from ParticleState
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
std::optional<Measurement> Detector::measure(ParticleState particleState) const {
  // Calling the other "measure" function to get the measurement
  return measure(particleState.position);
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
