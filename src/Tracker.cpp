// Header files needed
#include <TMatrixD.h>
#include <TMatrixDfwd.h>
#include <cmath>
#include <vector>

// Custom classes
#include "Tracker.hpp"
#include "MeasuresAndStates.hpp"
#include "PhysicalParameters.hpp"
#include "Utils.hpp"

// Namespaces
using namespace std;



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Global variables
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Initializing the state at 0, that is at the particle cannon 6 dimentional vector (t, x, y, 1/speed, thetazx, thetazy)
static constexpr double initialStateData[6] = {0., 0., 0., 1. / LIGHT_SPEED, 0., 0.};

static constexpr double bigT = VERY_HIGH_TIME_ERROR * VERY_HIGH_TIME_ERROR;
static constexpr double bigX = VERY_HIGH_SPACE_ERROR * VERY_HIGH_SPACE_ERROR;
static constexpr double bigVInv = VERY_HIGH_VELOCITY_INVERSE_ERROR * VERY_HIGH_VELOCITY_INVERSE_ERROR;
static constexpr double bigDirection = VERY_HIGH_DIRECTION_ERROR * VERY_HIGH_DIRECTION_ERROR;

static constexpr double initialStateSData[36] = {
    bigT, 0., 0., 0., 0., 0.,
    0., bigX, 0., 0., 0., 0.,
    0., 0., bigX, 0., 0., 0.,
    0., 0., 0., bigVInv, 0., 0.,
    0., 0., 0., 0., bigDirection, 0.,
    0., 0., 0., 0., 0., bigDirection};

static const TMatrixD initialStateValue(6, 1, initialStateData);
static const TMatrixD initialStateError(6, 6, initialStateSData);
static const MatrixStateEstimate initialState{initialStateValue, initialStateError};



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// estimateNextState
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MatrixStateEstimate Tracker::estimateNextState(const MatrixStateEstimate& preaviousState, double deltaZ) const {
  // Evolution matrix
  double evolutionMatrixData[36] = {
            1., 0., 0., deltaZ, 0.,     0.,
            0., 1., 0., 0.,     deltaZ, 0.,
            0., 0., 1., 0.,     0.,     deltaZ,
            0., 0., 0., 1.,     0.,     0.,
            0., 0., 0., 0.,     1.,     0.,
            0., 0., 0., 0.,     0.,     1.};

  TMatrixD evolutionMatrix(6, 6, evolutionMatrixData);
  TMatrixD estimatedStateValue = TMatrixD(evolutionMatrix, TMatrixD::kMult, preaviousState.value);

  // Evolution of inverse velocity
  double inverseVelocityEvolutionSigma = V_EVOLUTION_SIGMA_KALMAN * pow(estimatedStateValue(3,0), 2);

  // Evolution of uncertainty
  double evolutionUncertaintyData[36] = {
        TIME_EVOLUTION_SIGMA * TIME_EVOLUTION_SIGMA, 0., 0., 0., 0., 0.,
        0., SPACE_EVOLUTION_SIGMA * SPACE_EVOLUTION_SIGMA, 0., 0., 0., 0.,
        0., 0., SPACE_EVOLUTION_SIGMA* SPACE_EVOLUTION_SIGMA, 0., 0., 0.,
        0., 0., 0., inverseVelocityEvolutionSigma * inverseVelocityEvolutionSigma, 0., 0.,
        0., 0., 0., 0., DIRECTION_EVOLUTION_SIGMA * DIRECTION_EVOLUTION_SIGMA, 0.,
        0., 0., 0., 0., 0., DIRECTION_EVOLUTION_SIGMA * DIRECTION_EVOLUTION_SIGMA};

  TMatrixD evolutionUncertainty(6, 6, evolutionUncertaintyData);
  TMatrixD estimatedStateError = TMatrixD(evolutionMatrix, TMatrixD::kMult, TMatrixD(preaviousState.uncertainty, TMatrixD::kMultTranspose, evolutionMatrix));
  estimatedStateError += evolutionUncertainty;

  return MatrixStateEstimate{estimatedStateValue, estimatedStateError};
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// initializeFilterRealTime
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Tracker::initializeFilterRealTime(const vector<Measurement> &measures, vector<MatrixStateEstimate> &predictedStates, vector<MatrixStateEstimate> &filteredStates) const {
  // Predicted state
  double predictedData[6] = {measures[0].t, measures[0].x, measures[0].y, 1. / LIGHT_SPEED, 0., 0.};
  TMatrixD stateValue(6, 1, predictedData);

  // Measure uncertainty
  TMatrixD measureError = consideredDetectors[0].getMeasureUncertainty();

  double firstSdata[36] = {
    measureError(0, 0), 0., 0., 0., 0., 0.,
    0., measureError(1, 1), 0., 0., 0., 0.,
    0., 0., measureError(2, 2), 0., 0., 0.,
    0., 0., 0., bigVInv, 0., 0.,
    0., 0., 0., 0., bigDirection, 0.,
    0., 0., 0., 0., 0., bigDirection};

  TMatrixD stateError(6, 6, firstSdata);

  // Predicted and filtered states
  predictedStates.push_back(MatrixStateEstimate{initialStateValue, initialStateError});
  filteredStates.push_back(MatrixStateEstimate{stateValue, stateError});
  
  if (measures.size() == 1) return;

  // State
  const double deltaZ = consideredDetectors[1].getBottmLeftPosition().Z() - consideredDetectors[0].getBottmLeftPosition().Z();
  const double t = measures[1].t;
  const double x = measures[1].x;
  const double y = measures[1].y;

  // Variations
  TMatrixD preaviousStateValue = TMatrixD(filteredStates[1].value);
  const double deltaT = t - preaviousStateValue(0, 0);
  const double deltaX = x - preaviousStateValue(1, 0);
  const double deltaY = y - preaviousStateValue(2, 0);

  double data[6] = {t, x, y, deltaT / deltaZ, deltaX / deltaZ, deltaY / deltaZ};
  stateValue.SetMatrixArray(data);

  // Uncertainties
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
  predictedStates.push_back(MatrixStateEstimate{initialStateValue, initialStateError});
  filteredStates.push_back(MatrixStateEstimate{stateValue, stateError});
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// initializeFilter
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Tracker::initializeFilter(const vector<Measurement> &measures, vector<MatrixStateEstimate> &predictedStates, vector<MatrixStateEstimate> &filteredStates) const {
  if (measures.size() == 1) {
    initializeFilterRealTime(measures, predictedStates, filteredStates);
    return;
  }

  // Considered states
  const double t = measures[0].t;
  const double x = measures[0].x;
  const double y = measures[0].y;
  const double nextT = measures[1].t;
  const double nextX = measures[1].x;
  const double nextY = measures[1].y;

  // Variations
  const double deltaT = nextT - t;
  const double deltaX = nextX - x;
  const double deltaY = nextY - y;
  const double deltaZ = consideredDetectors[1].getBottmLeftPosition().Z() - consideredDetectors[0].getBottmLeftPosition().Z();

  // State
  double data[6] = {measures[0].t,   measures[0].x,   measures[0].y, deltaT / deltaZ, deltaX / deltaZ, deltaY / deltaZ};
  TMatrixD stateValue(6, 1, data);

  // Uncertainties
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

  predictedStates.push_back(MatrixStateEstimate{initialStateValue, initialStateError});
  filteredStates.push_back(MatrixStateEstimate{stateValue, stateError});
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// kalmanFilter
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
kalmanFilterResult Tracker::kalmanFilter(const vector<Measurement> &measures, bool logging, bool realTime) const {
  if (logging) cout << "KALMAN FILTER LOGS" << endl;
  vector<MatrixStateEstimate> filteredStates;
  vector<MatrixStateEstimate> predictedStates;

  // State at the particle gun
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
    // Measure
    double measureData[3] = {measures[i].t, measures[i].x, measures[i].y};
    TMatrixD measure(3, 1, measureData);
    TMatrixD measureError = consideredDetectors[i].getMeasureUncertainty();

    // Previous state
    TMatrixD preaviousStateValue = TMatrixD(filteredStates[i].value);
    TMatrixD preaviousStateError = TMatrixD(filteredStates[i].uncertainty);

    const double deltaZ = consideredDetectors[i].getBottmLeftPosition().Z() - consideredDetectors[i - 1].getBottmLeftPosition().Z();

    double projectiondata[18] = {1., 0., 0., 0., 0., 0., 0., 1., 0.,
                                 0., 0., 0., 0., 0., 1., 0., 0., 0.};
    TMatrixD projectionMatrix(3, 6, projectiondata);

    // Estimate next state
    MatrixStateEstimate estimatedNextState = estimateNextState(filteredStates[i], deltaZ);
    TMatrixD estimatedStateValue = TMatrixD(estimatedNextState.value);
    TMatrixD estimatedStateError = TMatrixD(estimatedNextState.uncertainty);

    // Residual
    TMatrixD residual = TMatrixD(measure, TMatrixD::kMinus, TMatrixD(projectionMatrix, TMatrixD::kMult, estimatedStateValue));

    // Kalman Gain
    TMatrixD kalmanGainDenominator = TMatrixD(projectionMatrix, TMatrixD::kMult, TMatrixD(estimatedStateError, TMatrixD::kMultTranspose, projectionMatrix));

    kalmanGainDenominator += measureError;
    kalmanGainDenominator.SetTol(DETERMINANT_TOLERANCE);
    kalmanGainDenominator.Invert();

    TMatrixD kalmanGain = TMatrixD(TMatrixD(estimatedStateError, TMatrixD::kMultTranspose, projectionMatrix), TMatrixD::kMult, kalmanGainDenominator);
    
    // Filtered state
    TMatrixD filteredStateValue = TMatrixD(kalmanGain, TMatrixD::kMult, residual);

    // Printouts for logging
    if (logging) {
      cout << "Residual: " << endl;
      Utils::printMatrix(residual);
      cout << endl;

      cout << "Preavious State Error" << endl;
      Utils::printMatrix(preaviousStateError);
      cout<<endl;
      
      cout << "Estimated State Error" << endl;
      Utils::printMatrix(estimatedStateError);
      cout<< endl;
      
      cout << "Kalman Gain" << endl;
      Utils::printMatrix(kalmanGain);
      cout << endl << endl << endl;
    }

    // Update filtered state
    filteredStateValue += estimatedStateValue;
    TMatrixD filteredStateError = TMatrixD(kalmanGain, TMatrixD::kMult, TMatrixD(projectionMatrix, TMatrixD::kMult, estimatedStateError));
    filteredStateError = TMatrixD(estimatedStateError, TMatrixD::kMinus, filteredStateError);

    predictedStates.push_back(MatrixStateEstimate{estimatedStateValue, estimatedStateError});
    filteredStates.push_back(MatrixStateEstimate{filteredStateValue, filteredStateError});
  }

  return kalmanFilterResult{predictedStates, filteredStates};
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// kalmanSmoother
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
vector<MatrixStateEstimate> Tracker::kalmanSmoother(const vector<MatrixStateEstimate> &filteredStates, bool logging) const {
  if (logging) {
    cout << "KALMAN SMOOTHER LOGS" << endl;
  }
  
  // Smoothed state vector
  vector<MatrixStateEstimate> smoothedStates;
  smoothedStates.push_back(filteredStates.back());

  // Evolution matrix
  TMatrixD evolutionMatrix(6,6);

  // Initializing the first state
  for (int i = (int)filteredStates.size() - 2; i > -1; i--) {
    TMatrixD smoothedNextStateValue = TMatrixD(smoothedStates.back().value);
    TMatrixD smoothedNextStateError = TMatrixD(smoothedStates.back().uncertainty);

    // NOTE: This indexes are like this because filteredStates has an element corresponding to the initial state (i.e. at z=0)
    const double deltaZ = i != 0
                            ? consideredDetectors[i].getBottmLeftPosition().Z() - consideredDetectors[i - 1].getBottmLeftPosition().Z()
                            : consideredDetectors[i].getBottmLeftPosition().Z();

    double evolutiondata[36] = {
            1., 0., 0., deltaZ, 0., 0.,
            0., 1., 0., 0., deltaZ, 0.,
            0., 0., 1., 0., 0., deltaZ,
            0., 0., 0., 1., 0., 0.,
            0., 0., 0., 0., 1., 0.,
            0., 0., 0., 0., 0., 1.};

    evolutionMatrix.SetMatrixArray(evolutiondata);

    // Estimation of next state
    MatrixStateEstimate estimatedNextState = estimateNextState(filteredStates[i], deltaZ);
    TMatrixD estimatedNextStateValue = TMatrixD(estimatedNextState.value);
    TMatrixD estimatedNextStateError = TMatrixD(estimatedNextState.uncertainty);
    TMatrixD estimatedNextStateErrorInverted = TMatrixD(estimatedNextStateError);
    estimatedNextStateErrorInverted.SetTol(DETERMINANT_TOLERANCE);

    // Printouts for logging
    if (logging) {
      cout << "Evolution Matrix" << endl;
      Utils::printMatrix(evolutionMatrix);
      cout << endl;

      cout << "Firtered state" << endl;
      Utils::printMatrix(filteredStates[i].uncertainty);
      cout << endl;
      
      cout << "Estimated next state error" << endl;
      Utils::printMatrix(estimatedNextStateErrorInverted);
      cout << endl << endl;
    }

    estimatedNextStateErrorInverted.Invert();

    // Smoother Gain
    TMatrixD smootherGain = TMatrixD(filteredStates[i].uncertainty, TMatrixD::kMultTranspose, evolutionMatrix);

    // Printouts for logging
    if (logging) {
      cout << "Gain first part" << endl;
      Utils::printMatrix(smootherGain);
      cout << endl;
    }

    smootherGain = TMatrixD(smootherGain, TMatrixD::kMult, estimatedNextStateErrorInverted);

    // Residual
    TMatrixD residualValue = TMatrixD(smoothedNextStateValue, TMatrixD::kMinus, estimatedNextStateValue);

    // Smoothed state
    TMatrixD smoothedStateValue = TMatrixD(smootherGain, TMatrixD::kMult, residualValue);
    smoothedStateValue += filteredStates[i].value;

    // Uncertainties
    TMatrixD residualError = TMatrixD(smoothedNextStateError, TMatrixD::kMinus,estimatedNextStateError);
    TMatrixD smoothedStateError = TMatrixD(residualError, TMatrixD::kMultTranspose, smootherGain);
    smoothedStateError = TMatrixD(smootherGain, TMatrixD::kMult, smoothedStateError);
    smoothedStateError += filteredStates[i].uncertainty;

    // Printouts for logging
    if (logging) {
      cout << "Estimated next state error inverted" << endl;
      Utils::printMatrix(estimatedNextStateErrorInverted);
      cout << endl;

      cout << "Smoother gain" << endl;
      Utils::printMatrix(smootherGain);
      cout << endl;

      cout << "Residual error" << endl;
      Utils::printMatrix(residualError);
      cout << endl;

      cout << "Residual value" << endl;
      Utils::printMatrix(residualValue);
      cout << endl << endl;
    }

    smoothedStates.push_back(MatrixStateEstimate{smoothedStateValue, smoothedStateError});
  }

  std::reverse(smoothedStates.begin(), smoothedStates.end());
  return smoothedStates;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// computeChi2s
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Chi2Variables Tracker::computeChi2s(const vector<ParticleState> &expectedStates, const vector<MatrixStateEstimate> &obtainedStates, bool logging, bool skipFirst) const {
  // Variables initialization
  double tChi2 = 0;
  double xChi2 = 0;
  double yChi2 = 0;
  double vChi2 = 0;
  double xzChi2 = 0;
  double yzChi2 = 0;

  // Initial value for the computation
  int initialIndex = (skipFirst) ? 1 : 0;

  // NOTE: The vectors are traversed backwords because sometimes the first elements are not computed
  for (int i = initialIndex; i < (int)obtainedStates.size(); i++) {
      tChi2 += pow((expectedStates[i].position.T() - obtainedStates[i].value(0,0))/sqrt(obtainedStates[i].uncertainty(0,0)), 2);
      xChi2 += pow((expectedStates[i].position.X() - obtainedStates[i].value(1,0))/sqrt(obtainedStates[i].uncertainty(1,1)), 2);
      yChi2 += pow((expectedStates[i].position.Y() - obtainedStates[i].value(2,0))/sqrt(obtainedStates[i].uncertainty(2,2)), 2);
      vChi2 += pow(((1./expectedStates[i].velocity.Z()) - obtainedStates[i].value(3,0))/sqrt(obtainedStates[i].uncertainty(3,3)), 2);
      xzChi2 += pow(((expectedStates[i].velocity.X()/expectedStates[i].velocity.Z()) - obtainedStates[i].value(4,0))/sqrt(obtainedStates[i].uncertainty(4,4)), 2);
      yzChi2 += pow(((expectedStates[i].velocity.Y()/expectedStates[i].velocity.Z()) - obtainedStates[i].value(5,0))/sqrt(obtainedStates[i].uncertainty(5,5)), 2);
  }

  // Printouts for logging
  if (logging) {
    int dof = (skipFirst) ? obtainedStates.size() - 7 : obtainedStates.size() - 6;

    cout <<"t =     " << tChi2 <<  "  | dof = " << dof << "  | pvalue = " <<TMath::Prob(tChi2, dof) << endl;
    cout <<"x =     " << xChi2 <<  "  | dof = " << dof << "  | pvalue = " <<TMath::Prob(xChi2, dof) << endl;
    cout <<"y =     " << yChi2 <<  "  | dof = " << dof << "  | pvalue = " <<TMath::Prob(yChi2, dof) << endl;
    cout <<"1/v_z = " << vChi2 <<  "  | dof = " << dof << "  | pvalue = " <<TMath::Prob(vChi2, dof) << endl;
    cout <<"xz =    " << xzChi2 << "  | dof = " << dof << "  | pvalue = " <<TMath::Prob(xzChi2, dof) << endl;
    cout <<"yz =    " << yzChi2 << "  | dof = " << dof << "  | pvalue = " <<TMath::Prob(yzChi2, dof) << endl;
  }

  return Chi2Variables{tChi2, xChi2, yChi2, vChi2, xzChi2, yzChi2};
}
