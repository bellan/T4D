#pragma once

namespace Coordinates {

/**
 * The standard three-position
 */
class tPosition{
public:
    double x;
    double y;
    double z;
    
    tPosition(double x=0., double y=0., double z=0.): x(x), y(y), z(z) {}
    
    tPosition operator* (double lambda) const;
    void operator+= (tPosition other);
    void operator-= (tPosition other);
    void operator*= (double lambda);
};

tPosition operator+ (tPosition left, tPosition right);
tPosition operator- (tPosition left, tPosition right);
}
