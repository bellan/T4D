#pragma once

constexpr int NUMBER_OF_PARTICLES = 10000;

/**
 * PROGRAM PARAMETERS
 * NOTE: this paramater should be chosen such that the standard detector
 * uncertainty is much smaller but wont be erased by this (6 orders of magnitude
 * bigger should be ok)
 */
constexpr double VERY_HIGH_TIME_ERROR = 1e-6;
constexpr double VERY_HIGH_SPACE_ERROR = 1;
constexpr double VERY_HIGH_VELOCITY_INVERSE_ERROR = 1e-4;
constexpr double VERY_HIGH_DIRECTION_ERROR = 1e4;
constexpr double DETERMINANT_TOLERANCE = 1e-80;

// PHISICAL CONSTANTS
constexpr double FOUNDAMENTAL_CHARGE = 1.602176634e-19;
constexpr double LIGHT_SPEED = 299792458.;

// PARTICLE CONSTANTS
constexpr double MIN_PARTICLE_MASS =
    (0.5e6 * FOUNDAMENTAL_CHARGE) / (LIGHT_SPEED * LIGHT_SPEED);
constexpr double MAX_PARTICLE_MASS =
    (300e6 * FOUNDAMENTAL_CHARGE) / (LIGHT_SPEED * LIGHT_SPEED);

constexpr double MIN_BETA = 0.8;
constexpr double MAX_BETA = 1.;

// DETECTOR PARAMETERS
constexpr int NUMBER_OF_DETECTORS = 8;
constexpr double DISTANCE_BETWEEN_DETECTORS = 1.e-2;
// NOTE: This parameter should be used to limit the movement along
// the x and y axes. It can be smaller than the real dimension
constexpr double DETECTOR_DIMENSION = 1.e-3;
constexpr double DETECTOR_SPACE_UNCERTAINTY = 1e-6;
constexpr double DETECTOR_TIME_UNCERTAINTY = 1e-11;

// GUN PARAMETERS (not used in this version)
constexpr double MIN_TIME_BETWEEN_PARTICLE =
    (NUMBER_OF_DETECTORS * DISTANCE_BETWEEN_DETECTORS * 1.1) / LIGHT_SPEED;

/**
 * EVOLUTION PARAMETERS
 * NOTE: The following parameters are used both in the data generation and in
 * the tracking process. They represent a random noise introduced in the
 * evolution of the particle. There are various processes that contribute to
 * this noise, but the main two are
 *  - Multiple scattering: a process inside the detector that offsets the
 * position of the particle.
 *  - Energy loss due to particle-matter interaction occourring inside
 * structural parts of the experiment (likely steel plates between detectors).
 *  Therefore there should be noise introduced in the 3D position and in the
 * velocty modulus.
 */
constexpr double TIME_EVOLUTION_SIGMA = 0.; // Old value 5.e-12
constexpr double SPACE_EVOLUTION_SIGMA = 5.e-7;

/**
 *  NOTE: These next two parameters should be modified toghether somewhat
 *  euristically.
 *  The first regulates the data generation, while the second the tracking.
 *  The theoretical value of the second is
 *  (VELOCITY_EVOLUTION_SIGMA / (LIGHT_SPEED * LIGHT_SPEED))
 *  however the value should be finetuned to optimize the tracking.
 */
/**
 * Realistic value of a muon (m = 105.65 MeV/c^2) at the ionization minimum
 * passing through 1 mm of steel (rho = 7.7 g/cm^3)
 *  Delta E = 0.1*7.7*2 MeV = 1.54 MeV
 *  Delta v = -0.01468 c
 */
constexpr double VELOCITY_EVOLUTION_SIGMA = 0.01468 * LIGHT_SPEED;
// constexpr double INVERSE_VELOCITY_EVOLUTION_SIGMA = 0.2 / LIGHT_SPEED;
constexpr double DIRECTION_EVOLUTION_SIGMA = 0.; // Old value 0.5e-4
