#pragma once

#include <optional>

#include "Position.hpp"

struct Measurement {
   double x;
   double y;
   double t;
   int detectorID;
};

class Detector {
public:
    Detector(double zPosition=0., double width=0., double height=0., double depth=0.):
        width(width),
        height(height),
        depth(),
        zPosition()
    {}

    double getZPosition() const { return zPosition; }
    double getWidth() const { return width; }
    std::optional<Measurement> measure(Coordinates::Position particlePosition) const;
    

private:
    static int counter;

    double width;
    double height;
    double depth;
    double zPosition;
    int id;
};
