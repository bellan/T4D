#include "Tracker.hpp"
#include "Detector.hpp"
#include <TMatrix.h>
#include <TMatrixDBasefwd.h>
#include <TMatrixDfwd.h>
#include <vector>

// Returns the states
std::vector<State> Tracker::fromMeasuresToStates(std::vector<Measurement> measurements) {
    std::vector<State> states;
    for (Measurement measure : measurements) {
        states.push_back(experimentalSetup.detectors[measure.detectorID].fromMeasureToState(measure));
    }
    return states;
}

std::vector<State> Tracker::kalmanFilter(std::vector<State> unfilteredStates) {
    std::vector<State> filteredStates(unfilteredStates.size() + 1);

    TMatrixD evolutionMatrix(6,6);

    // Initializing the state at 0, that is at the particle cannon
    TMatrixD initialStateValue(6,1); // 8 dimentional vector (4 position and 4 momentum)
    TMatrixD initialStateError(6,6);
    double data[8] = {0.,0.,0.,0.,0.,0.};
    double sdata[64] = {1.e18,0.,0.,0.,0.,0.,0.,0.,
                        0.,1.e18,0.,0.,0.,0.,0.,0.,
                        0.,0.,1.e18,0.,0.,0.,0.,0.,
                        0.,0.,0.,1.e18,0.,0.,0.,0.,
                        0.,0.,0.,0.,1.e18,0.,0.,0.,
                        0.,0.,0.,0.,0.,1.e18,0.,0.,
                        0.,0.,0.,0.,0.,0.,1.e18,0.,
                        0.,0.,0.,0.,0.,0.,0.,1.e18};
    initialStateValue.SetMatrixArray(data,"");
    initialStateError.SetMatrixArray(sdata, "");
    filteredStates.push_back(State{initialStateError, initialStateError});

    // Initializing the first state
    for (int i = 0; i< unfilteredStates.size(); i++) {
        TMatrixD preaviousStateValue = filteredStates[i].value;
        TMatrixD preaviousStateError = filteredStates[i].uncertainty;

        const double deltaZ = experimentalSetup.detectors[i].getBottmLeftPosition().Z() - experimentalSetup.detectors[i-1].getBottmLeftPosition().Z() ?
            i != 0 : experimentalSetup.detectors[i].getBottmLeftPosition().Z();
        double evolutiondata[36] = {1.,0.,0.,0.,0.,0., // TODO: Fix this (it is not linear)
                                    0.,1.,0.,0.,deltaZ,0.,
                                    0.,0.,1.,0.,0.,deltaZ,
                                    0.,0.,0.,1.,0.,0.,
                                    0.,0.,0.,0.,1.,0.,
                                    0.,0.,0.,0.,0.,1.};
        evolutionMatrix.SetMatrixArray(evolutiondata);

        TMatrixD estimatedStateValue = TMatrixD(evolutionMatrix, TMatrixD::kMult, preaviousStateValue);
        TMatrixD estimatedStateError = TMatrixD(evolutionMatrix, TMatrixD::kMult, TMatrixD(preaviousStateError, TMatrixD::kMultTranspose, evolutionMatrix));

        TMatrixD kalmanGainDenominator = TMatrixD(estimatedStateError, TMatrixD::kPlus, preaviousStateError).Invert();
        TMatrixD filteredStateValue = TMatrixD(unfilteredStates[i].value, TMatrixD::kMinus, estimatedStateValue);
        filteredStateValue = TMatrixD(kalmanGainDenominator, TMatrixD::kMult, filteredStateValue);
        filteredStateValue = TMatrixD(estimatedStateError, TMatrixD::kMult, filteredStateValue);
        filteredStateValue += estimatedStateValue;

        TMatrixD filteredStateError = TMatrixD(kalmanGainDenominator, TMatrixD::kMult, estimatedStateError);
        filteredStateError = TMatrixD(estimatedStateError, TMatrixD::kMult, filteredStateError);
        filteredStateError = TMatrixD(estimatedStateError, TMatrixD::kMinus, filteredStateError);

        filteredStates.push_back(State{filteredStateValue, filteredStateError});
    }

    return filteredStates;
}

// States Tracker::kalmanSmoother(States filteredStates) {
//     std::vector<TMatrixD> filteredValues = filteredStates.values;
//     std::vector<TMatrixD> filteredErrors = filteredStates.uncertainties;
//
//     std::vector<TMatrixD> smoothedValues(filteredValues.size());
//     std::vector<TMatrixD> smoothedErrors(filteredErrors.size());
//
//     TMatrixD evolutionMatrix(8,8);
//     // TODO: This should be with time intervals
//     double evolutiondata[64] = {1.,0.,0.,0.,1.,0.,0.,0.,
//                                 0.,1.,0.,0.,0.,1.,0.,0.,
//                                 0.,0.,1.,0.,0.,0.,1.,0.,
//                                 0.,0.,0.,1.,0.,0.,0.,1.,
//                                 0.,0.,0.,0.,1.,0.,0.,0.,
//                                 0.,0.,0.,0.,0.,1.,0.,0.,
//                                 0.,0.,0.,0.,0.,0.,1.,0.,
//                                 0.,0.,0.,0.,0.,0.,0.,1.};
//     evolutionMatrix.SetMatrixArray(evolutiondata);
//
//     for (int i = filteredValues.size() - 2; i > -1; i--) {
//         TMatrixD smoothedState = 
//     }
//
//
//
//
// }
