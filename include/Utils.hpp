#pragma once

#include "Detector.hpp"
#include "MeasuresAndStates.hpp"

#include <TMatrixD.h>

namespace Utils {

/**
 * Print a TMatrixD to stdout.
 *
 * @param matrix the matrix to be printed.
 */
void printMatrix(TMatrixD matrix);

/**
 * Divide the measurement Vector into a vector of vectors containing measures
 * from a single particle for each vector.
 *
 * It first orders the measures based on the time they occour. Then devides them
 * based on the detector id: if the detector of a measure is lower than that of
 * the preavious measure, this is registered as a new particle.
 *
 * NOTE: This algorithm assumes that before a particle is shot the preavious one
 * has already passed through all the detectors
 *
 * @param allMeasures the vector containing the complete sequence of
 *                    measurements.
 * @param (out)(optional) particlesNumber the number of the particles
 *
 * @return a vector of vector each one representing a single particle
 */
std::vector<std::vector<Measurement>>
separateMeasuresInParticles(const std::vector<Measurement> &allMeasures);

/**
 * Concatenates a vector of vectors into one single vector.
 *
 * @param separateMeasures the vector of vectors to be concatenated.
 */
std::vector<Measurement> concatenateMeasures(
    const std::vector<std::vector<Measurement>> &separateMeasures);

/**
 * Save all the produced and filtered data to a csv file.
 *
 * @param detectors the detectors of the experiment.
 * @param theoreticalStates the theoretical states of the particles (i.e. the
 *                          states if multiple scattering was inactive).
 * @param realStates the states of the particles with multiple scattering
 *                   active.
 * @param measures the registered measures.
 * @param predictedStates the states predicted by the kalman filter.
 * @param filteredStates the states filtered by the kalman filter.
 * @param smoothedStates the states smoothed by the kalman smoother.
 */
void saveDataToCSV(
    const std::vector<Detector> &detectors,
    const std::vector<std::vector<ParticleState>> &theoreticalStates,
    const std::vector<std::vector<ParticleState>> &realStates,
    const std::vector<std::vector<Measurement>> &measures,
    const std::vector<std::vector<MatrixStateEstimate>> &predictedStates,
    const std::vector<std::vector<MatrixStateEstimate>> &filteredStates,
    const std::vector<std::vector<MatrixStateEstimate>> &smoothedStates);

/**
 * Save all the produced and filtered data to a csv file.
 *
 * @param detectors the detectors of the experiment.
 * @param theoreticalStates the theoretical states of the particles (i.e. the
 *                          states if multiple scattering was inactive).
 * @param realStates the states of the particles with multiple scattering
 *                   active.
 * @param measures the registered measures.
 * @param predictedStates the states predicted by the kalman filter.
 * @param filteredStates the states filtered by the kalman filter.
 * @param smoothedStates the states smoothed by the kalman smoother.
 */
void saveDataToCSV(
    const std::vector<Detector> &detectors,
    const std::vector<std::vector<ParticleState>> &realStates,
    const std::vector<std::vector<Measurement>> &measures,
    const std::vector<std::vector<MatrixStateEstimate>> &smoothedStates);

/**
 * Print elapsed time in human readable format
 * 
 * @param btime begin time.
 * @param etime end time.
 */
void printTime(float btime, float etime);

}
