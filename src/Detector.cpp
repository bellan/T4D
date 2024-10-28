#include "Detector.hpp"

#include "MeasuresAndStates.hpp"
#include "PhisicalParameters.hpp"
#include "RandomGenerator.hpp"

#include <TLorentzVector.h>
#include <TMatrixD.h>
#include <cmath>
#include <optional>

int Detector::counter = 0;

Detector::Detector(double zPosition, double width, double height)
    : id{counter}, width{width}, height{height},
      bottomLeftPosition{-width / 2., -height / 2., zPosition} {
  counter++;
}

std::optional<Measurement>
Detector::measure(TLorentzVector particlePosition) const {
  const double deltaX = particlePosition.X();
  const double deltaY = particlePosition.Y();
  const double deltaZ = particlePosition.Z() - this->bottomLeftPosition.z();

  const bool xConstrain = deltaX > bottomLeftPosition.x() &&
                          deltaX < bottomLeftPosition.x() + width;
  const bool yConstrain = deltaY > bottomLeftPosition.y() &&
                          deltaY < bottomLeftPosition.y() + height;
  const bool zConstrain = deltaZ == 0;

  RandomGenerator &randomGenerator = RandomGenerator::getInstance();
  const double measuredT = randomGenerator.generateGaussian(
      particlePosition.T(), DETECTOR_TIME_UNCERTAINTY);
  const double measuredX =
      randomGenerator.generateGaussian(deltaX, DETECTOR_SPACE_UNCERTAINTY);
  const double measuredY =
      randomGenerator.generateGaussian(deltaY, DETECTOR_SPACE_UNCERTAINTY);
  return (xConstrain && yConstrain && zConstrain)
             ? std::optional<Measurement>{{measuredT, measuredX, measuredY, id}}
             : std::nullopt;
}

std::optional<Measurement> Detector::measure(TMatrixD particleState) const {
  const double t = particleState(0, 0);
  const double x = particleState(1, 0);
  const double y = particleState(2, 0);

  const TLorentzVector particlePosition{x, y, this->bottomLeftPosition.z(), t};

  return measure(particlePosition);
}

std::optional<Measurement>
Detector::measure(ParticleState particleState) const {
  return measure(particleState.position);
}

TMatrixD Detector::getMeasureUncertainty() const {
  double sdata[36] = {
      DETECTOR_TIME_UNCERTAINTY * DETECTOR_TIME_UNCERTAINTY,   0., 0.,
      0., DETECTOR_SPACE_UNCERTAINTY * DETECTOR_SPACE_UNCERTAINTY, 0.,
      0., 0., DETECTOR_SPACE_UNCERTAINTY * DETECTOR_SPACE_UNCERTAINTY};
  TMatrixD uncertainty(3, 3, sdata);
  return uncertainty;
}
