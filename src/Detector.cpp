#include "Detector.hpp"

#include <optional>
#include <TLorentzVector.h>

/**
 * The detector counter.
 * It is used to calculate the id of the detectors.
 */
int Detector::counter = 0;

/**
 * The constructor.
 * It determines the position from the zPosition (since they are all aligned to the z-axis i.d. position=(0,0,z))
 */
Detector::Detector(double zPosition, double width, double height):
id{counter},
width{width},
height{height},
bottomLeftPosition{-width/2.,-height/2.,zPosition} {
    counter++;
}

/**
 * Creates a Measurement from a particlePosition, if the particle is inside the area of the detector.
 *
 * @param particlePosition the position of the particle.
 *
 * @return an optional measurement. It contains the measure if the particle was inside, nullopt otherwise.
 */
std::optional<Measurement> Detector::measure(TLorentzVector particlePosition) const {
    const double deltaX = particlePosition.X();
    const double deltaY = particlePosition.Y();
    const double deltaZ = particlePosition.Z() - this->bottomLeftPosition.z();

    const bool xConstrain = deltaX > bottomLeftPosition.x() && deltaX < bottomLeftPosition.x() + width;
    const bool yConstrain = deltaY > bottomLeftPosition.y() && deltaY < bottomLeftPosition.y() + height;
    const bool zConstrain = deltaZ == 0;

    return (xConstrain && yConstrain && zConstrain) ? std::optional<Measurement>{{particlePosition.T(), deltaX, deltaY, id}} : std::nullopt;
}


TMatrixD Detector::generateMeasureToStateMatrix() const {
    // TODO: Fix the z position, undoable with matrix multiplication
    TMatrixD measureToStateMatrix(8,3);
    double data[24] = {1.,0.,0.,
                       0.,1.,0.,
                       0.,0.,1.,
                       0.,0.,0.,
                       0.,0.,0.,
                       0.,0.,0.,
                       0.,0.,0.,
                       0.,0.,0.};
    measureToStateMatrix.SetMatrixArray(data,"");
    return measureToStateMatrix;
}

TMatrixD Detector::getUncertaintyMatrix() const {
    // TODO: Set reasonable values to uncertainties
    TMatrixD uncertaintyMatrix(3,3);
    double data[24] = {1.,0.,0.,
                       0.,1.,0.,
                       0.,0.,1.};
    uncertaintyMatrix.SetMatrixArray(data,"");
    return uncertaintyMatrix;
}
