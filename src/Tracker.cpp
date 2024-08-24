#include "Tracker.hpp"

#include "MeasuresAndStates.hpp"
#include "PhisicalParameters.hpp"
#include "Utils.hpp"

#include <TMatrixD.h>
#include <vector>

kalmanFilterResult
Tracker::kalmanFilter(const std::vector<Measurement> &measures, bool logging) const {
  if (logging) std::cout<<"KALMAN FILTER LOGS"<<std::endl;
  std::vector<MatrixStateEstimate> filteredStates;
  std::vector<MatrixStateEstimate> predictedStates;

  TMatrixD evolutionMatrix(6, 6);

  // Initializing the state at 0, that is at the particle cannon
  // 6 dimentional vector (t,x,y,1/speed,thetazx,thetazy)
  double initialStateData[6] = {0., 0., 0., 1. / LIGHT_SPEED, 0., 0.};
  constexpr double bigT = VERY_HIGH_TIME_ERROR * VERY_HIGH_TIME_ERROR;
  constexpr double bigX = VERY_HIGH_SPACE_ERROR * VERY_HIGH_SPACE_ERROR;
  constexpr double bigVInv =
      VERY_HIGH_VELOCITY_INVERSE_ERROR * VERY_HIGH_VELOCITY_INVERSE_ERROR;
  constexpr double bigDirection =
      VERY_HIGH_DIRECTION_ERROR * VERY_HIGH_DIRECTION_ERROR;
  double initialStateSData[36] = {
      bigT, 0., 0., 0., 0., 0.,
      0., bigX, 0., 0., 0., 0.,
      0., 0., bigX, 0., 0., 0.,
      0., 0., 0., bigVInv, 0., 0.,
      0., 0., 0., 0., bigDirection, 0.,
      0., 0., 0., 0., 0., bigDirection};

  TMatrixD initialStateValue(6, 1, initialStateData);
  TMatrixD initialStateError(6, 6, initialStateSData);
  MatrixStateEstimate initialState{initialStateValue, initialStateError};

  predictedStates.push_back(initialState);
  filteredStates.push_back(initialState);

  // Initializing the first state
  for (int i = 0; i < (int)measures.size(); i++) {
    if (i == 0) {
      double predictedData[6] = {
          measures[i].t, measures[i].x, measures[i].y, 1. / LIGHT_SPEED, 0., 0.};
      TMatrixD stateValue(6, 1, predictedData);

      TMatrixD measureError = detectors[i].getMeasureUncertainty();
      double sdata[36] = {
          measureError(0, 0), 0., 0., 0., 0., 0.,
          0., measureError(1, 1), 0., 0., 0., 0.,
          0., 0., measureError(2, 2), 0., 0., 0.,
          0., 0., 0., bigVInv, 0., 0.,
          0., 0., 0., 0., bigDirection, 0.,
          0., 0., 0., 0., 0., bigDirection};
      TMatrixD stateError(6, 6, sdata);
      predictedStates.push_back(
          MatrixStateEstimate{initialStateValue, initialStateError});
      filteredStates.push_back(MatrixStateEstimate{stateValue, stateError});
      continue;
    } else if (i == 1) {
      const double deltaZ = detectors[i].getBottmLeftPosition().Z() -
                            detectors[i - 1].getBottmLeftPosition().Z();
      const double t = measures[i].t;
      const double x = measures[i].x;
      const double y = measures[i].y;
      TMatrixD preaviousStateValue = TMatrixD(filteredStates[i].value);
      const double deltaT = t - preaviousStateValue(0, 0);
      const double deltaX = x - preaviousStateValue(1, 0);
      const double deltaY = y - preaviousStateValue(2, 0);
      double data[6] = {measures[i].t,   measures[i].x,   measures[i].y,
                        deltaT / deltaZ, deltaX / deltaZ, deltaY / deltaZ};
      TMatrixD stateValue(6, 1, data);

      TMatrixD measureError = detectors[i].getMeasureUncertainty();
      TMatrixD preaviousStateError = TMatrixD(filteredStates[i].uncertainty);
      const double sDeltaT2 = measureError(0, 0) + preaviousStateError(0, 0);
      const double sDeltaX2 = measureError(1, 1) + preaviousStateError(1, 1);
      const double sDeltaY2 = measureError(2, 2) + preaviousStateError(2, 2);

      double sdata[36] = {measureError(0, 0),           0., 0., 0., 0., 0., 0.,
                          measureError(1, 1),           0., 0., 0., 0., 0., 0.,
                          measureError(2, 2),           0., 0., 0., 0., 0., 0.,
                          sDeltaT2 / (deltaZ * deltaZ), 0., 0., 0., 0., 0., 0.,
                          sDeltaX2 / (deltaZ * deltaZ), 0., 0., 0., 0., 0., 0.,
                          sDeltaY2 / (deltaZ * deltaZ)};
      TMatrixD stateError(6, 6, sdata);
      predictedStates.push_back(
          MatrixStateEstimate{initialStateValue, initialStateError});
      filteredStates.push_back(MatrixStateEstimate{stateValue, stateError});
      continue;
    }

    double measureData[3] = {measures[i].t, measures[i].x, measures[i].y};
    TMatrixD measure(3, 1, measureData);

    TMatrixD measureError = detectors[i].getMeasureUncertainty();

    TMatrixD preaviousStateValue = TMatrixD(filteredStates[i].value);
    TMatrixD preaviousStateError = TMatrixD(filteredStates[i].uncertainty);

    const double deltaZ = detectors[i].getBottmLeftPosition().Z() -
                          detectors[i - 1].getBottmLeftPosition().Z();
    double evolutionMatrixData[36] = {
            1., 0., 0., deltaZ, 0., 0.,
            0., 1., 0., 0., deltaZ, 0.,
            0., 0., 1., 0., 0., deltaZ,
            0., 0., 0., 1., 0., 0.,
            0., 0., 0., 0., 1., 0.,
            0., 0., 0., 0., 0., 1.};
    evolutionMatrix.SetMatrixArray(evolutionMatrixData);

    double projectiondata[18] = {1., 0., 0., 0., 0., 0., 0., 1., 0.,
                                 0., 0., 0., 0., 0., 1., 0., 0., 0.};
    TMatrixD projectionMatrix(3, 6, projectiondata);

    // TODO: Check if this is correct
    constexpr double inverse_velocity_sigma =
        DIRECTION_EVOLUTION_SIGMA / (LIGHT_SPEED * LIGHT_SPEED);
    double evolutionUncertaintyData[36] = {
        pow(TIME_EVOLUTION_SIGMA, 2), 0., 0., 0., 0., 0.,
        0., pow(SPACE_EVOLUTION_SIGMA, 2), 0., 0., 0., 0.,
        0., 0., pow(SPACE_EVOLUTION_SIGMA, 2), 0., 0., 0.,
        0., 0., 0., pow(inverse_velocity_sigma, 2), 0., 0.,
        0., 0., 0., 0., pow(DIRECTION_EVOLUTION_SIGMA, 2), 0.,
        0., 0., 0., 0., 0., pow(DIRECTION_EVOLUTION_SIGMA, 2)};
    TMatrixD evolutionUncertainty(6, 6, evolutionUncertaintyData);

    // NOTE: see https://arxiv.org/abs/2101.12040 for the math
    TMatrixD estimatedStateValue =
        TMatrixD(evolutionMatrix, TMatrixD::kMult, preaviousStateValue);
    TMatrixD estimatedStateError =
        TMatrixD(evolutionMatrix, TMatrixD::kMult,
                 TMatrixD(preaviousStateError, TMatrixD::kMultTranspose,
                          evolutionMatrix));
    estimatedStateError += evolutionUncertainty;

    TMatrixD residual = TMatrixD(
        measure, TMatrixD::kMinus,
        TMatrixD(projectionMatrix, TMatrixD::kMult, estimatedStateValue));
    TMatrixD kalmanGainDenominator =
        TMatrixD(projectionMatrix, TMatrixD::kMult,
                 TMatrixD(estimatedStateError, TMatrixD::kMultTranspose,
                          projectionMatrix));
    kalmanGainDenominator += measureError;
    kalmanGainDenominator.SetTol(DETERMINANT_TOLERANCE);
    kalmanGainDenominator.Invert();

    TMatrixD kalmanGain =
        TMatrixD(TMatrixD(estimatedStateError, TMatrixD::kMultTranspose,
                          projectionMatrix),
                 TMatrixD::kMult, kalmanGainDenominator);
    TMatrixD filteredStateValue =
            TMatrixD(kalmanGain, TMatrixD::kMult, residual);
    if (logging) {
      std::cout<<"residual"<<std::endl;
      Utils::printMatrix(residual);
      std::cout<<std::endl;
      std::cout<<"PreaviousStateError"<<std::endl;
      Utils::printMatrix(preaviousStateError);
      std::cout<<std::endl;
      std::cout<<"estimatedStateError"<<std::endl;
      Utils::printMatrix(estimatedStateError);
      std::cout<<std::endl;
      std::cout<<"kalmanGain"<<std::endl;
      Utils::printMatrix(kalmanGain);
      std::cout<<std::endl;
    }
    filteredStateValue += estimatedStateValue;

    TMatrixD filteredStateError = TMatrixD(
        kalmanGain, TMatrixD::kMult,
        TMatrixD(projectionMatrix, TMatrixD::kMult, estimatedStateError));
    filteredStateError =
        TMatrixD(estimatedStateError, TMatrixD::kMinus, filteredStateError);

    predictedStates.push_back(
        MatrixStateEstimate{estimatedStateValue, estimatedStateError});
    filteredStates.push_back(
        MatrixStateEstimate{filteredStateValue, filteredStateError});
        std::cout<<"\n"<<std::endl;
  }

  return kalmanFilterResult{predictedStates, filteredStates};
}

std::vector<MatrixStateEstimate>
Tracker::kalmanSmoother(const std::vector<MatrixStateEstimate> &filteredStates, bool logging) const {
  if (logging) std::cout<<"KALMAN SMOOTHER LOGS"<<std::endl;
  std::vector<MatrixStateEstimate> smoothedStates;

  TMatrixD evolutionMatrix(6, 6);

  smoothedStates.push_back(filteredStates.back());

  // Initializing the first state
  for (int i = (int)filteredStates.size() - 2; i > -1; i--) {
    TMatrixD smoothedNextStateValue = TMatrixD(smoothedStates.back().value);
    TMatrixD smoothedNextStateError =
        TMatrixD(smoothedStates.back().uncertainty);

    // NOTE: This indexes are like this because filteredStates has an element
    // corresponding to the initial state (i.e. at z=0)
    const double deltaZ = i != 0
                              ? detectors[i].getBottmLeftPosition().Z() -
                                    detectors[i - 1].getBottmLeftPosition().Z()
                              : detectors[i].getBottmLeftPosition().Z();
    double evolutiondata[36] = {1., 0.,     0., deltaZ, 0., 0., 0., 1., 0.,
                                0., deltaZ, 0., 0.,     0., 1., 0., 0., deltaZ,
                                0., 0.,     0., 1.,     0., 0., 0., 0., 0.,
                                0., 1.,     0., 0.,     0., 0., 0., 0., 1.};
    evolutionMatrix.SetMatrixArray(evolutiondata);

    TMatrixD estimatedNextStateValue =
        TMatrixD(evolutionMatrix, TMatrixD::kMult, filteredStates[i].value);
    TMatrixD estimatedNextStateError =
        TMatrixD(evolutionMatrix, TMatrixD::kMult,
                 TMatrixD(filteredStates[i].uncertainty,
                          TMatrixD::kMultTranspose, evolutionMatrix));

    TMatrixD estimatedNextStateErrorInverted =
        TMatrixD(estimatedNextStateError);
    estimatedNextStateErrorInverted.SetTol(DETERMINANT_TOLERANCE);
    estimatedNextStateErrorInverted.Invert();

    TMatrixD smootherGain = TMatrixD(filteredStates[i].uncertainty,
                                     TMatrixD::kMultTranspose, evolutionMatrix);
    smootherGain = TMatrixD(smootherGain, TMatrixD::kMultTranspose,
                            estimatedNextStateErrorInverted);

    TMatrixD residualValue = TMatrixD(smoothedNextStateValue, TMatrixD::kMinus,
                                      estimatedNextStateValue);
    TMatrixD smoothedStateValue =
        TMatrixD(smootherGain, TMatrixD::kMult, residualValue);
    smoothedStateValue += filteredStates[i].value;

    TMatrixD residualError = TMatrixD(smoothedNextStateError, TMatrixD::kMinus,
                                      estimatedNextStateError);
    TMatrixD smoothedStateError =
        TMatrixD(residualError, TMatrixD::kMultTranspose, smootherGain);
    smoothedStateError =
        TMatrixD(smootherGain, TMatrixD::kMult, smoothedStateError);
    smoothedStateError += filteredStates[i].uncertainty;

    smoothedStates.push_back(
        MatrixStateEstimate{smoothedStateValue, smoothedStateError});
  }

  std::reverse(smoothedStates.begin(), smoothedStates.end());
  return smoothedStates;
}
