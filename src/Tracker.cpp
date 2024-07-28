#include "Tracker.hpp"
#include "Detector.hpp"
#include "PhisicalParameters.hpp"
#include "Utils.hpp"
#include <TMatrixD.h>
#include <ostream>
#include <vector>

// Returns the states
// std::vector<State> Tracker::fromMeasuresToStates(std::vector<Measurement> measurements) {
//     std::vector<State> states;
//     for (Measurement measure : measurements) {
//         states.push_back(experimentalSetup.detectors[measure.detectorID].fromMeasureToState(measure));
//     }
//     return states;
// }

std::vector<State> Tracker::kalmanFilter(std::vector<Measurement> measures, std::vector<State> &predictions) {
    std::vector<State> filteredStates;
    predictions = std::vector<State>();

    TMatrixD evolutionMatrix(6,6);

    // Initializing the state at 0, that is at the particle cannon
    TMatrixD initialStateValue(6,1); // 6 dimentional vector (t,x,y,1/speed,thetazx,thetazy)
    TMatrixD initialStateError(6,6);
    double data[6] = {0.,0.,0.,0.,0.,0.};
    constexpr double bigT = VERY_HIGH_TIME_ERROR * VERY_HIGH_TIME_ERROR;
    constexpr double bigX = VERY_HIGH_SPACE_ERROR * VERY_HIGH_SPACE_ERROR;
    double sdata[36] = {bigT,0.,0.,0.,0.,0.,
                        0.,bigX,0.,0.,0.,0.,
                        0.,0.,bigX,0.,0.,0.,
                        0.,0.,0.,bigX,0.,0.,
                        0.,0.,0.,0.,bigX,0.,
                        0.,0.,0.,0.,0.,bigX};
    initialStateValue.SetMatrixArray(data,"");
    initialStateError.SetMatrixArray(sdata, "");
    State state{initialStateValue, initialStateError};


    filteredStates.push_back(state);
    predictions.push_back(state);

    // Initializing the first state
    for (int i = 0; i < (int)measures.size(); i++) {
        double data[3] = {measures[i].t, measures[i].x, measures[i].y};

        TMatrixD measure(3, 1, data);
        TMatrixD measureError = experimentalSetup.detectors[i].getMeasureUncertainty();
        std::cout<<std::endl;


        TMatrixD preaviousStateValue = TMatrixD(filteredStates[i].value);
        TMatrixD preaviousStateError = TMatrixD(filteredStates[i].uncertainty);

        const double deltaZ = i != 0 ? experimentalSetup.detectors[i].getBottmLeftPosition().Z() - experimentalSetup.detectors[i-1].getBottmLeftPosition().Z()
             : experimentalSetup.detectors[i].getBottmLeftPosition().Z();
        double evolutiondata[36] = {1.,0.,0.,deltaZ,0.,0.,
                                    0.,1.,0.,0.,deltaZ,0.,
                                    0.,0.,1.,0.,0.,deltaZ,
                                    0.,0.,0.,1.,0.,0.,
                                    0.,0.,0.,0.,1.,0.,
                                    0.,0.,0.,0.,0.,1.};
        evolutionMatrix.SetMatrixArray(evolutiondata);

        double projectiondata[18] = {1., 0., 0., 0., 0., 0.,
                                     0., 1., 0., 0., 0., 0.,
                                     0., 0., 1., 0., 0., 0.};
        TMatrixD projectionMatrix(3, 6, projectiondata);

        // NOTE: see https://www.sciencedirect.com/science/article/pii/S0010465521001387 for the math
        TMatrixD estimatedStateValue = TMatrixD(evolutionMatrix, TMatrixD::kMult, preaviousStateValue);
        TMatrixD estimatedStateError = TMatrixD(evolutionMatrix, TMatrixD::kMult, TMatrixD(preaviousStateError, TMatrixD::kMultTranspose, evolutionMatrix));

        TMatrixD residual = TMatrixD(measure, TMatrixD::kMinus, TMatrixD(projectionMatrix, TMatrixD::kMult, estimatedStateValue));
        TMatrixD kalmanGainDenominator = TMatrixD(projectionMatrix, TMatrixD::kMult, TMatrixD(estimatedStateError, TMatrixD::kMultTranspose, projectionMatrix));
        kalmanGainDenominator += measureError;
        kalmanGainDenominator.Invert();

        TMatrixD kalmanGain = TMatrixD(TMatrixD(estimatedStateError, TMatrixD::kMultTranspose, projectionMatrix), TMatrixD::kMult, kalmanGainDenominator);
        TMatrixD filteredStateValue = TMatrixD(kalmanGain, TMatrixD::kMult, residual);
        filteredStateValue += estimatedStateValue;

        TMatrixD filteredStateError = TMatrixD(kalmanGain, TMatrixD::kMult, TMatrixD(projectionMatrix, TMatrixD::kMult, estimatedStateError));
        filteredStateError = TMatrixD(estimatedStateError, TMatrixD::kMinus, filteredStateError);

        printMatrix(filteredStateError);
        predictions.push_back(State{estimatedStateValue, estimatedStateError});
        filteredStates.push_back(State{filteredStateValue, filteredStateError});
    }

    return filteredStates;
}

std::vector<State> Tracker::kalmanSmoother(std::vector<State> filteredStates) {
    std::vector<State> smoothedStates;

    TMatrixD evolutionMatrix(6,6);

    smoothedStates.push_back(filteredStates.back());

    // Initializing the first state
    for (int i = (int)filteredStates.size() - 2; i > -1; i--) {
        TMatrixD smoothedNextStateValue = TMatrixD(smoothedStates.back().value);
        TMatrixD smoothedNextStateError = TMatrixD(smoothedStates.back().uncertainty);

        // NOTE: This indexes are like this because filteredStates has an element corresponding to the initial state (i.e. at z=0)
        const double deltaZ = i != 0 ? experimentalSetup.detectors[i].getBottmLeftPosition().Z() - experimentalSetup.detectors[i-1].getBottmLeftPosition().Z() :
            experimentalSetup.detectors[i].getBottmLeftPosition().Z();
        double evolutiondata[36] = {1.,0.,0.,deltaZ,0.,0.,
                                    0.,1.,0.,0.,deltaZ,0.,
                                    0.,0.,1.,0.,0.,deltaZ,
                                    0.,0.,0.,1.,0.,0.,
                                    0.,0.,0.,0.,1.,0.,
                                    0.,0.,0.,0.,0.,1.};
        evolutionMatrix.SetMatrixArray(evolutiondata);

        TMatrixD estimatedNextStateValue = TMatrixD(evolutionMatrix, TMatrixD::kMult, filteredStates[i].value);
        TMatrixD estimatedNextStateError = TMatrixD(evolutionMatrix, TMatrixD::kMult, TMatrixD(filteredStates[i].uncertainty, TMatrixD::kMultTranspose, evolutionMatrix));
        TMatrixD estimatedNextStateErrorInverted = TMatrixD(estimatedNextStateError).Invert();

        TMatrixD smootherGain = TMatrixD(filteredStates[i].uncertainty, TMatrixD::kMultTranspose, evolutionMatrix);
        smootherGain = TMatrixD(smootherGain, TMatrixD::kMultTranspose, estimatedNextStateErrorInverted);

        TMatrixD residualValue = TMatrixD(smoothedNextStateValue, TMatrixD::kMinus, estimatedNextStateValue);
        TMatrixD smoothedStateValue = TMatrixD(smootherGain, TMatrixD::kMult, residualValue);
        smoothedStateValue += filteredStates[i].value;

        TMatrixD residualError = TMatrixD(smoothedNextStateError, TMatrixD::kMinus, estimatedNextStateError);
        TMatrixD smoothedStateError = TMatrixD(residualError, TMatrixD::kMultTranspose, smootherGain);
        smoothedStateError = TMatrixD(smootherGain, TMatrixD::kMult, smoothedStateError);
        smoothedStateError += filteredStates[i].uncertainty;

        smoothedStates.push_back(State{smoothedStateValue, smoothedStateError});
    }

    std::reverse(smoothedStates.begin(), smoothedStates.end());
    return smoothedStates;
}
