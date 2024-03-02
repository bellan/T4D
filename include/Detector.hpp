#pragma once

#include "tPosition.hpp"
class Detector {
public:
    Detector(double width=0., double height=0., Coordinates::tPosition position=Coordinates::tPosition()):
        width(width),
        height(height),
        position(position) {}

private:
    double width;
    double height;
    Coordinates::tPosition position;
};
