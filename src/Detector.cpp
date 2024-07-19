#include "Detector.hpp"

#include <TMatrixDfwd.h>
#include <cmath>
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

State Detector::fromMeasureToState(Measurement measure) const {
    TMatrixD stateValue(6, 1);
    double data[6] = {measure.t, measure.x, measure.y, 0., 0., 0.};
    stateValue.SetMatrixArray(data);

    TMatrixD stateUncertainty(6, 6);
    // TODO: Set reasonable values to uncertainties (maybe as a class data member)
    double sdata[49] = {1., 0., 0., 0., 0., 0.,
                        0., 1., 0., 0., 0., 0.,
                        0., 0., 1., 0., 0., 0.,
                        0., 0., 0., 1.e18, 0., 0.,
                        0., 0., 0., 0., 1.e18, 0.,
                        0., 0., 0., 0., 0., 1.e18};
    stateUncertainty.SetMatrixArray(sdata);


    return State{stateValue, stateUncertainty};
}


State Detector::fromMeasureToState(Measurement currentMeasure, State preaviousState) const {
    TMatrixD stateValue(6, 1);

    const double deltaT = currentMeasure.t - preaviousState.value(0,0);
    const double deltaX = (currentMeasure.x - preaviousState.value(1,0));
    const double deltaY = (currentMeasure.y - preaviousState.value(2,0));
    const double deltaZ = (bottomLeftPosition.Z() - preaviousState.value(3,0)) / deltaT;
    const double speedX = deltaX / deltaT;
    const double speedY = deltaY / deltaT;
    const double speedZ = deltaZ / deltaT;

    const double speed = sqrt(speedX*speedX + speedY*speedY + speedZ*speedZ);
    const double tanThetaXZ = deltaX/deltaZ;
    const double tanThetaYZ = deltaY/deltaZ;

    double data[6] = {currentMeasure.t, currentMeasure.x, currentMeasure.y, speed, tanThetaXZ, tanThetaXZ};
    stateValue.SetMatrixArray(data);

    TMatrixD stateUncertainty(6, 6);
    // TODO: Set reasonable values to uncertainties (maybe as a class data member)
    const double sT = 1.;
    const double sX = 1.;
    const double sY = 1.;
    const double sZ = 1.;
    const double sPreaviousT = preaviousState.uncertainty(0,0);
    const double sPreaviousX = preaviousState.uncertainty(1,1);
    const double sPreaviousY = preaviousState.uncertainty(2,2);
    const double sPreaviousZ = preaviousState.uncertainty(3,3);
    const double sDeltaT = sqrt(sT*sT + sPreaviousT*sPreaviousT);
    const double sDeltaX = sqrt(sX*sX + sPreaviousX*sPreaviousX);
    const double sDeltaY = sqrt(sY*sY + sPreaviousY*sPreaviousY);
    const double sDeltaZ = sqrt(sZ*sZ + sPreaviousZ*sPreaviousZ);
    const double sSpeedX = sqrt(pow(sDeltaX/deltaT,2) + pow(sDeltaT*speedX/deltaT,2));
    const double sSpeedY = sqrt(pow(sDeltaY/deltaT,2) + pow(sDeltaT*speedY/deltaT,2));
    const double sSpeedZ = sqrt(pow(sDeltaZ/deltaT,2) + pow(sDeltaT*speedZ/deltaT,2));
    const double sSpeed = 1./speed * sqrt(pow(2.*speedX*sSpeedX,2) + pow(2.*speedY*sSpeedY,2) + pow(2.*speedZ*sSpeedZ,2));
    const double sTanThetaXZ = sqrt(pow(sSpeedX/speedZ,2) + pow(speedX*sSpeedZ/(speedZ*speedZ),2));
    const double sTanThetaYZ = sqrt(pow(sSpeedY/speedZ,2) + pow(speedY*sSpeedZ/(speedZ*speedZ),2));

    double sdata[36] = {sT, 0., 0., 0., 0., 0.,
                        0., sX, 0., 0., 0., 0.,
                        0., 0., sY, 0., 0., 0.,
                        0., 0., 0., sSpeed, 0., 0.,
                        0., 0., 0., 0., sTanThetaXZ, 0.,
                        0., 0., 0., 0., 0., sTanThetaXZ};
    stateUncertainty.SetMatrixArray(sdata);

    return State{stateValue, stateUncertainty};
}
