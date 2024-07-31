#pragma once

#include "Detector.hpp"
#include "MeasuresAndStates.hpp"

#include <TMatrixD.h>

void printMatrix(TMatrixD matrix);

void saveDataToCSV(
    std::vector<Detector> detectors,
    std::vector<std::vector<ParticleState>> generatedStates,
    std::vector<std::vector<MatrixStateEstimate>> filteredStates,
    std::vector<std::vector<MatrixStateEstimate>> smoothedStates,
    std::vector<std::vector<MatrixStateEstimate>> predictedStates);
