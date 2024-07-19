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
    std::vector<TMatrixD> states(measurements.size() + 1);
    std::vector<TMatrixD> statesErrors(measurements.size() + 1);

    TMatrixD evolutionMatrix(8,8);
    // TODO: This should be with time intervals
    double evolutiondata[64] = {1.,0.,0.,0.,1.,0.,0.,0.,
                                0.,1.,0.,0.,0.,1.,0.,0.,
                                0.,0.,1.,0.,0.,0.,1.,0.,
                                0.,0.,0.,1.,0.,0.,0.,1.,
                                0.,0.,0.,0.,1.,0.,0.,0.,
                                0.,0.,0.,0.,0.,1.,0.,0.,
                                0.,0.,0.,0.,0.,0.,1.,0.,
                                0.,0.,0.,0.,0.,0.,0.,1.};
    evolutionMatrix.SetMatrixArray(evolutiondata);

    // Initializing the state at 0, that is at the particle cannon
    TMatrixD initialState(8,1); // 8 dimentional vector (4 position and 4 momentum)
    TMatrixD initialStateErrors(8,8);
    double data[8] = {0.,0.,0.,0.,0.,0.,0.,0.};
    double sdata[64] = {1.e8,0.,0.,0.,0.,0.,0.,0.,
                        0.,1.e8,0.,0.,0.,0.,0.,0.,
                        0.,0.,1.e8,0.,0.,0.,0.,0.,
                        0.,0.,0.,1.e8,0.,0.,0.,0.,
                        0.,0.,0.,0.,1.e8,0.,0.,0.,
                        0.,0.,0.,0.,0.,1.e8,0.,0.,
                        0.,0.,0.,0.,0.,0.,1.e8,0.,
                        0.,0.,0.,0.,0.,0.,0.,1.e8};
    initialState.SetMatrixArray(data,"");
    initialStateErrors.SetMatrixArray(sdata, "");
    states.push_back(initialState);
    statesErrors.push_back(initialStateErrors);

    // Initializing the first state
    for (int i = 0; i< measurements.size(); i++) {
        Measurement measure = measurements[i];
        const int detectorIndex = measure.detectorID;
        TMatrixD measureVector(3,1);
        double data[3] = {measure.t, measure.x, measure.y};
        measureVector.SetMatrixArray(data);

        TMatrixD measureToStateMatrix = experimentalSetup.detectors[detectorIndex].generateMeasureToStateMatrix();
        TMatrixD measureUncertainty = experimentalSetup.detectors[detectorIndex].getUncertaintyMatrix();

        TMatrixD measuredState = TMatrixD(measureToStateMatrix, TMatrixD::kMult, measureVector);
        TMatrixD measuredStateError = TMatrixD(measureToStateMatrix, TMatrixD::kMult, measureUncertainty);

        // TODO: Maybe add the momentum measure?? 

        if (i == 0) {
            states.push_back(measuredState);
            statesErrors.push_back(measuredStateError);
            continue;
        }

        TMatrixD predictedState = TMatrixD(evolutionMatrix, TMatrixD::kMult, states[i]); // NOTE: It's not i-1 because states has the cannon state at the beginning shifting everything by 1
        TMatrixD predictedError = TMatrixD(evolutionMatrix, TMatrixD::kMult, TMatrixD(statesErrors[i], TMatrixD::kMultTranspose, evolutionMatrix));

        TMatrixD kalmanGainDenominator = TMatrixD(measureToStateMatrix, TMatrixD::kMult, TMatrixD(predictedError, TMatrixD::kMultTranspose, measureToStateMatrix));
        kalmanGainDenominator += measureUncertainty;

        TMatrixD filteredState = measureVector;
        filteredState -= TMatrixD(measureToStateMatrix, TMatrixD::kMult, predictedState);
        filteredState = TMatrixD(kalmanGainDenominator.Invert(), TMatrixD::kMult, filteredState);
        filteredState = TMatrixD(predictedError, TMatrixD::kMult, TMatrixD(TMatrixD(TMatrixD::kTransposed, measureToStateMatrix), TMatrixD::kMult, filteredState));
        filteredState += predictedState;

        TMatrix filteredError = predictedError;
        filteredError = TMatrixD(kalmanGainDenominator.Invert(), TMatrixD::kMult, TMatrixD(measureToStateMatrix, TMatrixD::kMult, filteredError));
        filteredError = TMatrixD(predictedError, TMatrixD::kMult, TMatrixD(TMatrixD(TMatrixD::kTransposed, measureToStateMatrix), TMatrixD::kMult, filteredError));
        filteredError = TMatrixD(predictedError, TMatrixD::kMinus, filteredError);

        states.push_back(filteredState);
        statesErrors.push_back(filteredError);
    }

    return States(states, statesErrors);
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
