#pragma once

#include "Detector.hpp"
#include "MeasuresAndStates.hpp"

#include <TMatrixD.h>

void printMatrix(TMatrixD matrix);

std::vector<std::vector<Measurement>>
separateMeasuresInParticles(const std::vector<Measurement> &allMeasures);

void saveDataToCSV(
    const std::vector<Detector> &detectors,
    const std::vector<std::vector<ParticleState>> &theoreticalStates,
    const std::vector<std::vector<ParticleState>> &realStates,
    const std::vector<std::vector<Measurement>> &measures,
    const std::vector<std::vector<MatrixStateEstimate>> &predictedStates,
    const std::vector<std::vector<MatrixStateEstimate>> &filteredStates,
    const std::vector<std::vector<MatrixStateEstimate>> &smoothedStates);
