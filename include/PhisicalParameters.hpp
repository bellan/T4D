#pragma once

constexpr int NUMBER_OF_PARTICLES = 1;

// Program parameters
// NOTE: this paramater should be chosen such that the standard detector
// uncertainty is much smaller but wont be erased by this (6 orders of magnitude
// bigger should be ok)
constexpr double VERY_HIGH_TIME_ERROR = 1e-6;
constexpr double VERY_HIGH_SPACE_ERROR = 1;
constexpr double VERY_HIGH_VELOCITY_INVERSE_ERROR = 1e-1;
constexpr double VERY_HIGH_DIRECTION_ERROR = 1e4;
constexpr double DETERMINANT_TOLERANCE = 1e-55;

// Phisical costants
constexpr double FOUNDAMENTAL_CHARGE = 1.602176634e-19;
constexpr double LIGHT_SPEED = 299792458.;

// Particle parameters
constexpr double MIN_PARTICLE_MASS =
    (0.5e6 * FOUNDAMENTAL_CHARGE) / (LIGHT_SPEED * LIGHT_SPEED);
constexpr double MAX_PARTICLE_MASS =
    (300e6 * FOUNDAMENTAL_CHARGE) / (LIGHT_SPEED * LIGHT_SPEED);

constexpr double MIN_BETA = 0.8;
constexpr double MAX_BETA = 1.;

// Detector parameters
constexpr int NUMBER_OF_DETECTORS = 6;
constexpr double DISTANCE_BETWEEN_DETECTORS = 5e-3;
constexpr double DETECTOR_DIMENSION = 1.;
constexpr double DETECTOR_SPACE_UNCERTAINTY = 1e-6;
constexpr double DETECTOR_TIME_UNCERTAINTY = 1e-11;

// Gun parameters
constexpr double MIN_TIME_BETWEEN_PARTICLE =
    (NUMBER_OF_DETECTORS * DISTANCE_BETWEEN_DETECTORS * 1.1) / LIGHT_SPEED;

// Evolution parameters
constexpr double TIME_EVOLUTION_SIGMA = 1e-11;
constexpr double SPACE_EVOLUTION_SIGMA = 1e-6;
constexpr double VELOCITY_EVOLUTION_SIGMA = 0.01 * LIGHT_SPEED;
constexpr double DIRECTION_EVOLUTION_SIGMA = 1e-6;
