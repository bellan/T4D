#include "Detector.hpp"
#include "MeasuresAndStates.hpp"
#include "PhisicalParameters.hpp"

#include <TLorentzVector.h>
#include <TMatrixD.h>
#include <cmath>
#include <optional>

/**
 * The detector counter.
 * It is used to calculate the id of the detectors.
 */
int Detector::counter = 0;

/**
 * The constructor.
 * It determines the position from the zPosition (since they are all aligned to
 * the z-axis i.d. position=(0,0,z))
 */
Detector::Detector(double zPosition, double width, double height)
    : id{counter}, width{width}, height{height},
      bottomLeftPosition{-width / 2., -height / 2., zPosition} {
  counter++;
}

/**
 * Creates a Measurement from a particlePosition, if the particle is inside the
 * area of the detector.
 *
 * @param particlePosition the position of the particle.
 *
 * @return an optional measurement. It contains the measure if the particle was
 * inside, nullopt otherwise.
 */
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

  return (xConstrain && yConstrain && zConstrain)
             ? std::optional<Measurement>{{particlePosition.T(), deltaX, deltaY,
                                           id}}
             : std::nullopt;
}

/**
 * Creates a Measurement from a particlePosition, if the particle is inside the
 * area of the detector.
 *
 * @param particlePosition the position of the particle.
 *
 * @return an optional measurement. It contains the measure if the particle was
 * inside, nullopt otherwise.
 */
std::optional<Measurement> Detector::measure(TMatrixD particleState) const {
  const double t = particleState(0, 0);
  const double x = particleState(1, 0);
  const double y = particleState(2, 0);

  const bool xConstrain =
      x > bottomLeftPosition.x() && x < bottomLeftPosition.x() + width;
  const bool yConstrain =
      y > bottomLeftPosition.y() && y < bottomLeftPosition.y() + height;

  return (xConstrain && yConstrain) ? std::optional<Measurement>{{t, x, y, id}}
                                    : std::nullopt;
}

/**
 * Creates a Measurement from a ParticleState, if the particle is inside the
 * area of the detector.
 *
 * @param particlePosition the position of the particle.
 *
 * @return an optional measurement. It contains the measure if the particle was
 * inside, nullopt otherwise.
 */
std::optional<Measurement>
Detector::measure(ParticleState particleState) const {
  const double t = particleState.position.T();
  const double x = particleState.position.X();
  const double y = particleState.position.Y();

  const bool xConstrain =
      x > bottomLeftPosition.x() && x < bottomLeftPosition.x() + width;
  const bool yConstrain =
      y > bottomLeftPosition.y() && y < bottomLeftPosition.y() + height;

  return (xConstrain && yConstrain) ? std::optional<Measurement>{{t, x, y, id}}
                                    : std::nullopt;
}

TMatrixD Detector::getMeasureUncertainty() {
  double sdata[36] = {
      DETECTOR_TIME_UNCERTAINTY * DETECTOR_TIME_UNCERTAINTY,   0., 0., 0.,
      DETECTOR_SPACE_UNCERTAINTY * DETECTOR_SPACE_UNCERTAINTY, 0., 0., 0.,
      DETECTOR_SPACE_UNCERTAINTY * DETECTOR_SPACE_UNCERTAINTY};
  TMatrixD uncertainty(3, 3, sdata);
  return uncertainty;
}
