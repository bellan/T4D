#pragma once

#include <TLorentzVector.h>
#include <TMatrixD.h>
#include <TMatrixDfwd.h>
#include <TVector3.h>
#include <optional>

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

struct State {
public:
    TMatrixD value;
    TMatrixD uncertainty;
};


/**
 * The detector class.
 *
 * It represents a detector in the experiment.
 * It can generate a Measurement given a particle position.
 */
class Detector {
public:
    Detector(double zPosition, double width, double height);

    TVector3 getBottmLeftPosition() const { return bottomLeftPosition; }
    double getWidth() const { return width; }
    double getHeight() const { return height; }

    static void resetCounter() { counter = 0; } // TODO: Consider removing this. It can be useful to instanciate differente experiments (e.g. one for the sim and one for the reconstruction).

    std::optional<Measurement> measure(TLorentzVector particlePosition) const;

    State fromMeasureToState(Measurement measure) const;
    State fromMeasureToState(Measurement currentMeasure, State preaviousState) const;

private:
    static int counter;

    int id;
    double width;
    double height;
    TVector3 bottomLeftPosition;
};
