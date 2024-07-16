#include "Tracker.hpp"
#include "Detector.hpp"
#include "Particle.hpp"
#include <TMatrixDBasefwd.h>
#include <TMatrixDfwd.h>
#include <vector>

std::vector<TMatrixD> Tracker::kalmanFilter(std::vector<Measurement> measurements) {
    std::vector<TMatrixD> states(measurements.size() + 1);
    states[0] = 

}

