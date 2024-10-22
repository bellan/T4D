#include "Tracker.hpp"

#include "MeasuresAndStates.hpp"
#include "PhisicalParameters.hpp"

#include "Utils.hpp"
#include <TMatrixD.h>
#include <TMatrixDfwd.h>
#include <cmath>
#include <vector>

// Initializing the state at 0, that is at the particle cannon
// 6 dimentional vector (t,x,y,1/speed,thetazx,thetazy)
constexpr double initialStateData[6] = {0., 0., 0., 1. / LIGHT_SPEED, 0., 0.};
constexpr double bigT = VERY_HIGH_TIME_ERROR * VERY_HIGH_TIME_ERROR;
constexpr double bigX = VERY_HIGH_SPACE_ERROR * VERY_HIGH_SPACE_ERROR;
constexpr double bigVInv =
    VERY_HIGH_VELOCITY_INVERSE_ERROR * VERY_HIGH_VELOCITY_INVERSE_ERROR;
constexpr double bigDirection =
    VERY_HIGH_DIRECTION_ERROR * VERY_HIGH_DIRECTION_ERROR;
constexpr double initialStateSData[36] = {
    bigT, 0., 0., 0., 0., 0.,
    0., bigX, 0., 0., 0., 0.,
    0., 0., bigX, 0., 0., 0.,
    0., 0., 0., bigVInv, 0., 0.,
    0., 0., 0., 0., bigDirection, 0.,
    0., 0., 0., 0., 0., bigDirection};
const TMatrixD initialStateValue(6, 1, initialStateData);
const TMatrixD initialStateError(6, 6, initialStateSData);
const MatrixStateEstimate initialState{initialStateValue, initialStateError};

/**
 * Estimate the next state of the particle after a distance deltaZ from a state preaviousState.
 *
 * This function uses the distance deltaZ to compute the evolution matrix to apply to the
 * preavious state. Then, an evolution error is applied to the uncertainty.
 *
 * @param preaviousState the state of the particle at the preavious measure.
 * @param deltaZ the distance covered by the particle.
 * @return the estimated new state of the particle.
 */
MatrixStateEstimate Tracker::estimateNextState(const MatrixStateEstimate& preaviousState, double deltaZ) const {
  double evolutionMatrixData[36] = {
            1., 0., 0., deltaZ, 0., 0.,
            0., 1., 0., 0., deltaZ, 0.,
            0., 0., 1., 0., 0., deltaZ,
            0., 0., 0., 1., 0., 0.,
            0., 0., 0., 0., 1., 0.,
            0., 0., 0., 0., 0., 1.};
  TMatrixD evolutionMatrix(6, 6, evolutionMatrixData);
  TMatrixD estimatedStateValue = TMatrixD(evolutionMatrix, TMatrixD::kMult, preaviousState.value);

  double inverseVelocityEvolutionSigma = VELOCITY_EVOLUTION_SIGMA * pow(estimatedStateValue(3,0), 2);
  /*double inverseVelocityEvolutionSigma = 3 * VELOCITY_EVOLUTION_SIGMA * pow(estimatedStateValue(3,0), 2);*/
  double evolutionUncertaintyData[36] = {
        pow(TIME_EVOLUTION_SIGMA, 2), 0., 0., 0., 0., 0.,
        0., pow(SPACE_EVOLUTION_SIGMA, 2), 0., 0., 0., 0.,
        0., 0., pow(SPACE_EVOLUTION_SIGMA, 2), 0., 0., 0.,
        0., 0., 0., pow(inverseVelocityEvolutionSigma, 2), 0., 0.,
        0., 0., 0., 0., pow(DIRECTION_EVOLUTION_SIGMA, 2), 0.,
        0., 0., 0., 0., 0., pow(DIRECTION_EVOLUTION_SIGMA, 2)};
  TMatrixD evolutionUncertainty(6, 6, evolutionUncertaintyData);

  TMatrixD estimatedStateError = TMatrixD(evolutionMatrix, TMatrixD::kMult, TMatrixD(preaviousState.uncertainty, TMatrixD::kMultTranspose, evolutionMatrix));
  estimatedStateError += evolutionUncertainty;

  return MatrixStateEstimate{estimatedStateValue, estimatedStateError};
}

void Tracker::initializeFilterRealTime(
    const std::vector<Measurement> &measures,
    std::vector<MatrixStateEstimate> &predictedStates,
    std::vector<MatrixStateEstimate> &filteredStates) const {
  double predictedData[6] = {
      measures[0].t, measures[0].x, measures[0].y, 1. / LIGHT_SPEED, 0., 0.};
  TMatrixD stateValue(6, 1, predictedData);

  TMatrixD measureError = consideredDetectors[0].getMeasureUncertainty();
  double firstSdata[36] = {
    measureError(0, 0), 0., 0., 0., 0., 0.,
    0., measureError(1, 1), 0., 0., 0., 0.,
    0., 0., measureError(2, 2), 0., 0., 0.,
    0., 0., 0., bigVInv, 0., 0.,
    0., 0., 0., 0., bigDirection, 0.,
    0., 0., 0., 0., 0., bigDirection};
  TMatrixD stateError(6, 6, firstSdata);
  predictedStates.push_back(
      MatrixStateEstimate{initialStateValue, initialStateError});
  filteredStates.push_back(MatrixStateEstimate{stateValue, stateError});
  if (measures.size() == 1) return;

  const double deltaZ = consideredDetectors[1].getBottmLeftPosition().Z() -
                        consideredDetectors[0].getBottmLeftPosition().Z();
  const double t = measures[1].t;
  const double x = measures[1].x;
  const double y = measures[1].y;
  TMatrixD preaviousStateValue = TMatrixD(filteredStates[1].value);
  const double deltaT = t - preaviousStateValue(0, 0);
  const double deltaX = x - preaviousStateValue(1, 0);
  const double deltaY = y - preaviousStateValue(2, 0);
  double data[6] = {t, x, y,
                    deltaT / deltaZ, deltaX / deltaZ, deltaY / deltaZ};
  stateValue.SetMatrixArray(data);

  measureError = consideredDetectors[1].getMeasureUncertainty();
  TMatrixD preaviousStateError = TMatrixD(filteredStates[1].uncertainty);
  const double sDeltaT2 = measureError(0, 0) + preaviousStateError(0, 0);
  const double sDeltaX2 = measureError(1, 1) + preaviousStateError(1, 1);
  const double sDeltaY2 = measureError(2, 2) + preaviousStateError(2, 2);

  double secondSdata[36] = {
    measureError(0, 0), 0., 0., 0., 0., 0.,
    0., measureError(1, 1), 0., 0., 0., 0.,
    0., 0., measureError(2, 2), 0., 0., 0.,
    0., 0., 0., sDeltaT2 / (deltaZ * deltaZ), 0., 0.,
    0., 0., 0., 0., sDeltaX2 / (deltaZ * deltaZ), 0.,
    0., 0., 0., 0., 0., sDeltaY2 / (deltaZ * deltaZ)};
  stateError.SetMatrixArray(secondSdata);
  predictedStates.push_back(
      MatrixStateEstimate{initialStateValue, initialStateError});
  filteredStates.push_back(MatrixStateEstimate{stateValue, stateError});
}

void Tracker::initializeFilter(
    const std::vector<Measurement> &measures,
    std::vector<MatrixStateEstimate> &predictedStates,
    std::vector<MatrixStateEstimate> &filteredStates) const {
  if (measures.size() == 1) {
    initializeFilterRealTime(measures, predictedStates, filteredStates); 
    return;
  }

  const double deltaZ = consideredDetectors[1].getBottmLeftPosition().Z() -
                        consideredDetectors[0].getBottmLeftPosition().Z();
  const double t = measures[0].t;
  const double x = measures[0].x;
  const double y = measures[0].y;
  const double nextT = measures[1].t;
  const double nextX = measures[1].x;
  const double nextY = measures[1].y;
  const double deltaT = nextT - t;
  const double deltaX = nextX - x;
  const double deltaY = nextY - y;
  double data[6] = {measures[0].t,   measures[0].x,   measures[0].y,
                    deltaT / deltaZ, deltaX / deltaZ, deltaY / deltaZ};
  TMatrixD stateValue(6, 1, data);

  TMatrixD measureError = consideredDetectors[0].getMeasureUncertainty();
  TMatrixD nextMeasureError = consideredDetectors[1].getMeasureUncertainty();
  const double sDeltaT2 = measureError(0, 0) + nextMeasureError(0, 0);
  const double sDeltaX2 = measureError(1, 1) + nextMeasureError(1, 1);
  const double sDeltaY2 = measureError(2, 2) + nextMeasureError(2, 2);

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
}

kalmanFilterResult
Tracker::kalmanFilter(const std::vector<Measurement> &measures, bool logging, bool realTime) const {
  if (logging) std::cout<<"KALMAN FILTER LOGS"<<std::endl;
  std::vector<MatrixStateEstimate> filteredStates;
  std::vector<MatrixStateEstimate> predictedStates;

  // State at the gun
  // TODO: Consider removing this if using root file to save results
  predictedStates.push_back(initialState);
  filteredStates.push_back(initialState);

  int firstMeasureIndex = realTime ? 2 : 1;
  if (realTime)
    initializeFilterRealTime(measures, predictedStates, filteredStates);
  else 
    initializeFilter(measures, predictedStates, filteredStates);

  // Initializing the first state
  for (int i = firstMeasureIndex; i < (int)measures.size(); i++) {
    double measureData[3] = {measures[i].t, measures[i].x, measures[i].y};
    TMatrixD measure(3, 1, measureData);

    TMatrixD measureError = consideredDetectors[i].getMeasureUncertainty();

    TMatrixD preaviousStateValue = TMatrixD(filteredStates[i].value);
    TMatrixD preaviousStateError = TMatrixD(filteredStates[i].uncertainty);

    const double deltaZ = consideredDetectors[i].getBottmLeftPosition().Z() -
                          consideredDetectors[i - 1].getBottmLeftPosition().Z();

    double projectiondata[18] = {1., 0., 0., 0., 0., 0., 0., 1., 0.,
                                 0., 0., 0., 0., 0., 1., 0., 0., 0.};
    TMatrixD projectionMatrix(3, 6, projectiondata);

    MatrixStateEstimate estimatedNextState = estimateNextState(filteredStates[i], deltaZ);
    TMatrixD estimatedStateValue = TMatrixD(estimatedNextState.value);
    TMatrixD estimatedStateError = TMatrixD(estimatedNextState.uncertainty);

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
      // Put here stuff to be logged
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
      std::cout<<"\n"<<std::endl;
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
  }

  return kalmanFilterResult{predictedStates, filteredStates};
}

std::vector<MatrixStateEstimate>
Tracker::kalmanSmoother(const std::vector<MatrixStateEstimate> &filteredStates, bool logging) const {
  if (logging) std::cout<<"KALMAN SMOOTHER LOGS"<<std::endl;
  std::vector<MatrixStateEstimate> smoothedStates;

  TMatrixD evolutionMatrix(6,6);

  smoothedStates.push_back(filteredStates.back());

  // Initializing the first state
  for (int i = (int)filteredStates.size() - 2; i > -1; i--) {
    TMatrixD smoothedNextStateValue = TMatrixD(smoothedStates.back().value);
    TMatrixD smoothedNextStateError =
        TMatrixD(smoothedStates.back().uncertainty);

    // NOTE: This indexes are like this because filteredStates has an element
    // corresponding to the initial state (i.e. at z=0)
    const double deltaZ = i != 0
                              ? consideredDetectors[i].getBottmLeftPosition().Z() -
                                    consideredDetectors[i - 1].getBottmLeftPosition().Z()
                              : consideredDetectors[i].getBottmLeftPosition().Z();
    double evolutiondata[36] = {
            1., 0., 0., deltaZ, 0., 0.,
            0., 1., 0., 0., deltaZ, 0.,
            0., 0., 1., 0., 0., deltaZ,
            0., 0., 0., 1., 0., 0.,
            0., 0., 0., 0., 1., 0.,
            0., 0., 0., 0., 0., 1.};
    evolutionMatrix.SetMatrixArray(evolutiondata);

    MatrixStateEstimate estimatedNextState = estimateNextState(filteredStates[i], deltaZ);
    TMatrixD estimatedNextStateValue = TMatrixD(estimatedNextState.value);
    TMatrixD estimatedNextStateError = TMatrixD(estimatedNextState.uncertainty);

    TMatrixD estimatedNextStateErrorInverted = TMatrixD(estimatedNextStateError);

    estimatedNextStateErrorInverted.SetTol(DETERMINANT_TOLERANCE);
    if (logging) {
      std::cout<<"evolution Matrix"<<std::endl;
      Utils::printMatrix(evolutionMatrix);
      std::cout<<std::endl;
      /*std::cout<<"firtered state"<<std::endl;*/
      /*Utils::printMatrix(filteredStates[i].uncertainty);*/
      /*std::cout<<std::endl;*/
      std::cout<<"estimated next state error"<<std::endl;
      Utils::printMatrix(estimatedNextStateErrorInverted);
      std::cout<<std::endl;
    }
    estimatedNextStateErrorInverted.Invert();

    TMatrixD smootherGain = TMatrixD(filteredStates[i].uncertainty,
                                     TMatrixD::kMultTranspose, evolutionMatrix);
    if (logging) {
      std::cout<<"gain first part"<<std::endl;
      Utils::printMatrix(smootherGain);
      std::cout<<std::endl;
    }
    smootherGain = TMatrixD(smootherGain, TMatrixD::kMult,
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
    if (logging) {
      std::cout<<"estimated next state error inverted"<<std::endl;
      Utils::printMatrix(estimatedNextStateErrorInverted);
      std::cout<<std::endl;
      std::cout<<"smoother gain"<<std::endl;
      Utils::printMatrix(smootherGain);
      std::cout<<std::endl;
      std::cout<<"residual error"<<std::endl;
      Utils::printMatrix(residualError);
      std::cout<<std::endl;
      std::cout<<"residual value"<<std::endl;
      Utils::printMatrix(residualValue);
      std::cout<<std::endl;
      std::cout<<std::endl;
    }

    smoothedStates.push_back(
        MatrixStateEstimate{smoothedStateValue, smoothedStateError});
  }

  std::reverse(smoothedStates.begin(), smoothedStates.end());
  return smoothedStates;
}

Chi2Variables Tracker::computeChi2s(const std::vector<ParticleState> &expectedStates, const std::vector<MatrixStateEstimate> &obtainedStates, bool logging, bool skipFirst) const {
    double tChi2 = 0;
    double xChi2 = 0;
    double yChi2 = 0;
    double vChi2 = 0;
    double xzChi2 = 0;
    double yzChi2 = 0;

    int initialIndex = (skipFirst) ? 1 : 0;
    // NOTE: The vectors are traversed backwords because sometimes the first elements aren't computed
    for (int i = initialIndex; i < (int)obtainedStates.size(); i++) {
        tChi2 += pow((expectedStates[i].position.T() - obtainedStates[i].value(0,0))/sqrt(obtainedStates[i].uncertainty(0,0)), 2);
        xChi2 += pow((expectedStates[i].position.X() - obtainedStates[i].value(1,0))/sqrt(obtainedStates[i].uncertainty(1,1)), 2);
        yChi2 += pow((expectedStates[i].position.Y() - obtainedStates[i].value(2,0))/sqrt(obtainedStates[i].uncertainty(2,2)), 2);
        vChi2 += pow(((1./expectedStates[i].velocity.Z()) - obtainedStates[i].value(3,0))/sqrt(obtainedStates[i].uncertainty(3,3)), 2);
        xzChi2 += pow(((expectedStates[i].velocity.X()/expectedStates[i].velocity.Z()) - obtainedStates[i].value(4,0))/sqrt(obtainedStates[i].uncertainty(4,4)), 2);
        yzChi2 += pow(((expectedStates[i].velocity.Y()/expectedStates[i].velocity.Z()) - obtainedStates[i].value(5,0))/sqrt(obtainedStates[i].uncertainty(5,5)), 2);
    }

    if (logging) {
        int dof = (skipFirst) ? obtainedStates.size() - 7 : obtainedStates.size() - 6;
        std::cout<<"t = "<<tChi2<<"  | dof = "<<dof<<"  | pvalue = "<<TMath::Prob(tChi2, dof)<<std::endl;
        std::cout<<"x = "<<xChi2<<"  | dof = "<<dof<<"  | pvalue = "<<TMath::Prob(xChi2, dof)<<std::endl;
        std::cout<<"y = "<<yChi2<<"  | dof = "<<dof<<"  | pvalue = "<<TMath::Prob(yChi2, dof)<<std::endl;
        std::cout<<"1/v_z = "<<vChi2<<"  | dof = "<<dof<<"  | pvalue = "<<TMath::Prob(vChi2, dof)<<std::endl;
        std::cout<<"xz = "<<xzChi2<<"  | dof = "<<dof<<"  | pvalue = "<<TMath::Prob(xzChi2, dof)<<std::endl;
        std::cout<<"yz = "<<yzChi2<<"  | dof = "<<dof<<"  | pvalue = "<<TMath::Prob(yzChi2, dof)<<std::endl;
    }

    return Chi2Variables{tChi2, xChi2, yChi2, vChi2, xzChi2, yzChi2};
}
