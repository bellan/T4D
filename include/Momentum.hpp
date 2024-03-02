#pragma once

namespace Coordinates{

/**
 * The relativistic 4-Momentum
 */
class Momentum {
public:
    double energy;
    double px;
    double py;
    double pz;

    Momentum(double energy=0., double px=0., double py=0., double pz=0.): energy(energy), px(px), py(py), pz(pz) {}
};
}
