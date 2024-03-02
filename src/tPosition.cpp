#include "tPosition.hpp"

using Coordinates::tPosition;

tPosition Coordinates::tPosition::operator* (double lambda) const {
    return tPosition(x*lambda, y*lambda, z*lambda);
}

void Coordinates::tPosition::operator+= (tPosition other) {
    x += other.x;
    y += other.y;
    z += other.z;
}

void Coordinates::tPosition::operator-= (tPosition other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
}

void Coordinates::tPosition::operator*= (double lambda) {
    x *= lambda;
    y *= lambda;
    z *= lambda;
}

tPosition Coordinates::operator+ (tPosition left, tPosition right) {
    return tPosition(left.x + right.x, left.y + right.y, left.z + right.z);
}

tPosition Coordinates::operator- (tPosition left, tPosition right) {
    return tPosition(left.x - right.x, left.y - right.y, left.z - right.z);
}
