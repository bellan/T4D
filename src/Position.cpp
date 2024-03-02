#include "Position.hpp"

using Coordinates::Position;

Position Coordinates::Position::operator* (double lambda) const {
    return Position(x*lambda, y*lambda, z*lambda);
}

void Coordinates::Position::operator+= (Position other) {
    x += other.x;
    y += other.y;
    z += other.z;
}

void Coordinates::Position::operator-= (Position other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
}

void Coordinates::Position::operator*= (double lambda) {
    x *= lambda;
    y *= lambda;
    z *= lambda;
}

Position Coordinates::operator+ (Position left, Position right) {
    return Position(left.x + right.x, left.y + right.y, left.z + right.z);
}

Position Coordinates::operator- (Position left, Position right) {
    return Position(left.x - right.x, left.y - right.y, left.z - right.z);
}

