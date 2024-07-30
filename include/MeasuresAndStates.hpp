#pragma once

#include <TLorentzVector.h>
#include <TMatrixD.h>
#include <TVector3.h>

/**
 * The measuremnt struct.
 *
 * It contains the data produced by the detector.
 */
struct Measurement {
    double t;
    double x;
    double y;
    int detectorID;
};

/**
 * The matrix rapresentation of the estimated state.
 *
 * It contains the value and the uncertainty of the estimate.
 */
struct MatrixStateEstimate {
public:
    TMatrixD value;
    TMatrixD uncertainty;
};

/**
 * The measuremnt struct.
 *
 * It contains the data produced by the detector.
 */
struct ParticleState {
    TLorentzVector position;
    TVector3 velocity;
};
