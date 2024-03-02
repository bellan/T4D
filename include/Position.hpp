#pragma once

namespace Coordinates{

/**
 * The relativistic 4-Position
 */
class Position {
public:
    double t;
    double x;
    double y;
    double z;

    Position(double t=0., double x=0., double y=0., double z=0.): t(t), x(x), y(y), z(z) {}

    Position operator* (double lambda) const;
    void operator+= (Position other);
    void operator-= (Position other);
    void operator*= (double lambda);
};

Position operator+ (Position left, Position right);
Position operator- (Position left, Position right);
}
