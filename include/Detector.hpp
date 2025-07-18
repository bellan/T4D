#pragma once

// Header files needed
#include <TLorentzVector.h>
#include <TMatrixD.h>
#include <TVector3.h>
#include <optional>

// Custom classes
#include "Measure.hpp"
#include "ParticleState.hpp"
//#include "Structs.hpp"

// Namespaces
//using namespace std;

/**
 * The detector class.
 *
 * It represents a detector in the experiment.
 * It can generate a Measurement given a particle position.
 */
class Detector {
public:
  /**
   * The constructor.
   * It determines the position from the zPosition (since they are all aligned to
   * the z-axis i.d. position=(0,0,z))
   */
  Detector(double zPosition, double width, double height);
  Detector(double zPosition, double width, double height, int ID);

  Detector(const Detector&) = default;

  virtual ~Detector();

  int getId() const { return id; }
  TVector3 getBottmLeftPosition() const { return bottomLeftPosition; }
  double getWidth() const { return width; }
  double getHeight() const { return height; }

  /**
   * Creates a Measurement from a particlePosition, if the particle is inside the
   * area of the detector.
   *
   * @param particlePosition the position of the particle.
   *
   * @return an optional measurement. It contains the measure if the particle was
   * inside, nullopt otherwise.
   */
  //std::optional<Measurement> measure(TLorentzVector particlePosition) const;
  std::optional<Measure> measure2(TLorentzVector position, unsigned int particleID) const;

  /**
   * Creates a Measurement from a particlePosition, if the particle is inside the
   * area of the detector.
   *
   * @param particlePosition the position of the particle.
   *
   * @return an optional measurement. It contains the measure if the particle was
   * inside, nullopt otherwise.
   */
  //std::optional<Measurement> measure(TMatrixD particleState) const;

  /**
   * Creates a Measurement from a ParticleState, if the particle is inside the
   * area of the detector.
   *
   * @param particlePosition the position of the particle.
   *
   * @return an optional measurement. It contains the measure if the particle was
   * inside, nullopt otherwise.
   */
  //std::optional<Measurement> measure(ParticleState particleState) const;
  std::optional<Measure> measure2(ParticleState state) const;

  /**
   * Return the uncertainty of the detector
   *
   * @return the covariance matrix of the detector
   */
  TMatrixD getMeasureUncertainty() const;

private:
  /**
   * The detector counter.
   * It is used to calculate the id of the detectors.
   */
  static int counter;

  int id;
  double width;
  double height;
  TVector3 bottomLeftPosition;
};
