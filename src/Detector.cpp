#include "Detector.hpp"

#include <optional>

#include "Position.hpp"

int Detector::counter = 0;

std::optional<Measurement> Detector::measure(Coordinates::Position particlePosition) const {
    const double deltaX = particlePosition.x;
    const double deltaY = particlePosition.y;
    const double deltaZ = particlePosition.z - zPosition;

    const bool xConstrain = deltaX > 0 && deltaX < width;
    const bool yConstrain = deltaY > 0 && deltaY < height;
    const bool zConstrain = deltaZ > 0 && deltaZ < depth;

    return (xConstrain && yConstrain && zConstrain) ? std::optional<Measurement>{{deltaX, deltaY, particlePosition.t, id}} : std::nullopt;
}
