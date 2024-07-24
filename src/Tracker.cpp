#include "Tracker.hpp"
#include "Detector.hpp"
#include <TMatrixD.h>
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
    std::vector<State> filteredStates;

    TMatrixD evolutionMatrix(6,6);

    // Initializing the state at 0, that is at the particle cannon
    TMatrixD initialStateValue(6,1); // 6 dimentional vector (t,x,y,speed,thetazx,thetazy)
    TMatrixD initialStateError(6,6);
    double data[6] = {0.,0.,0.,0.,0.,0.};
    double sdata[36] = {1.e18,0.,0.,0.,0.,0.,
                        0.,1.e18,0.,0.,0.,0.,
                        0.,0.,1.e18,0.,0.,0.,
                        0.,0.,0.,1.e18,0.,0.,
                        0.,0.,0.,0.,1.e18,0.,
                        0.,0.,0.,0.,0.,1.e18};
    initialStateValue.SetMatrixArray(data,"");
    initialStateError.SetMatrixArray(sdata, "");
    State state{initialStateValue, initialStateError};

    filteredStates.push_back(state);

    // Initializing the first state
    for (int i = 0; i < (int)unfilteredStates.size(); i++) {
        TMatrixD preaviousStateValue = TMatrixD(filteredStates[i].value);
        TMatrixD preaviousStateError = TMatrixD(filteredStates[i].uncertainty);

        const double deltaZ = i != 0 ? experimentalSetup.detectors[i].getBottmLeftPosition().Z() - experimentalSetup.detectors[i-1].getBottmLeftPosition().Z()
             : experimentalSetup.detectors[i].getBottmLeftPosition().Z();
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

std::vector<State> Tracker::kalmanSmoother(std::vector<State> filteredStates) {
    std::vector<State> smoothedStates;
    std::cout<<"filtrato Ultimo"<<filteredStates.back().value(1,0)<<std::endl;

    TMatrixD evolutionMatrix(6,6);

    smoothedStates.push_back(filteredStates.back());

    // Initializing the first state
    for (int i = (int)filteredStates.size() - 2; i > -1; i--) {
        TMatrixD smoothedNextStateValue = TMatrixD(smoothedStates.back().value);
        TMatrixD smoothedNextStateError = TMatrixD(smoothedStates.back().uncertainty);

        // NOTE: This indexes are like this because filteredStates has an element corresponding to the initial state (i.e. at z=0)
        const double deltaZ = i != 0 ? experimentalSetup.detectors[i].getBottmLeftPosition().Z() - experimentalSetup.detectors[i-1].getBottmLeftPosition().Z() :
            experimentalSetup.detectors[i].getBottmLeftPosition().Z();
        double evolutiondata[36] = {1.,0.,0.,0.,0.,0., // TODO: Fix this (it is not linear)
                                    0.,1.,0.,0.,deltaZ,0.,
                                    0.,0.,1.,0.,0.,deltaZ,
                                    0.,0.,0.,1.,0.,0.,
                                    0.,0.,0.,0.,1.,0.,
                                    0.,0.,0.,0.,0.,1.};
        evolutionMatrix.SetMatrixArray(evolutiondata);

        TMatrixD estimatedNextStateValue = TMatrixD(evolutionMatrix, TMatrixD::kMult, filteredStates[i].value);
        TMatrixD estimatedNextStateError = TMatrixD(evolutionMatrix, TMatrixD::kMult, TMatrixD(filteredStates[i].uncertainty, TMatrixD::kMultTranspose, evolutionMatrix));
        TMatrixD estimatedNextStateErrorInverted = TMatrixD(estimatedNextStateError).Invert();

        TMatrixD smoothedStateValue = TMatrixD(smoothedNextStateValue, TMatrixD::kMinus, estimatedNextStateValue);
        smoothedStateValue = TMatrixD(estimatedNextStateErrorInverted, TMatrixD::kMult, smoothedStateValue);
        smoothedStateValue = TMatrixD(TMatrixD(TMatrixD::kTransposed, evolutionMatrix), TMatrixD::kMult, smoothedStateValue);
        smoothedStateValue = TMatrixD(filteredStates[i].uncertainty, TMatrixD::kMult, smoothedStateValue);
        smoothedStateValue += filteredStates[i].value;

        TMatrixD smoothedStateError = TMatrixD(evolutionMatrix, TMatrixD::kMult, TMatrixD(TMatrixD::kTransposed, filteredStates[i].uncertainty));
        smoothedStateError = TMatrixD(TMatrixD(TMatrixD::kTransposed, estimatedNextStateErrorInverted), TMatrixD::kMult, smoothedStateError);
        smoothedStateError = TMatrixD(TMatrixD(smoothedNextStateError, TMatrixD::kMinus, estimatedNextStateError), TMatrixD::kMult, smoothedStateError);
        smoothedStateError = TMatrixD(estimatedNextStateErrorInverted, TMatrixD::kMult, smoothedStateError);
        smoothedStateError = TMatrixD(TMatrixD(TMatrixD::kTransposed, evolutionMatrix), TMatrixD::kMult, smoothedStateError);
        smoothedStateError = TMatrixD(filteredStates[i].uncertainty, TMatrixD::kMult, smoothedStateError);
        smoothedStateError += filteredStates[i].uncertainty;

        smoothedStates.push_back(State{smoothedStateValue, smoothedStateError});
    }

    std::reverse(smoothedStates.begin(), smoothedStates.end());
    return smoothedStates;
}
