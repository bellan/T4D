#include "Detector.hpp"

#include <optional>
#include <TLorentzVector.h>

/**
 * The detector counter.
 * It is used to calculate the id of the detectors.
 */
int Detector::counter = 0;

/**
 * The constructor.
 * It determines the position from the zPosition (since they are all aligned to the z-axis i.d. position=(0,0,z))
 */
Detector::Detector(double zPosition, double width, double height):
bottomLeftPosition{0,0,zPosition},
width{width},
height{height},
id{counter} {
    counter++;
}

/**
 * Creates a Measurement from a particlePosition, if the particle is inside the area of the detector.
 *
 * @param particlePosition the position of the particle.
 *
 * @return an optional measurement. It contains the measure if the particle was inside, nullopt otherwise.
 */
std::optional<Measurement> Detector::measure(TLorentzVector particlePosition) const {
    const double deltaX = particlePosition.X();
    const double deltaY = particlePosition.Y();
    const double deltaZ = particlePosition.Z() - this->bottomLeftPosition.z();

    const bool xConstrain = deltaX > 0 && deltaX < width;
    const bool yConstrain = deltaY > 0 && deltaY < height;
    const bool zConstrain = deltaZ == 0;

    return (xConstrain && yConstrain && zConstrain) ? std::optional<Measurement>{{particlePosition.T(), deltaX, deltaY, id}} : std::nullopt;
}
