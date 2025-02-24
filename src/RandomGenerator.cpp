// Header files needed
#include <cstddef>
#include <ctime>

// Custom classes
#include "RandomGenerator.hpp"



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// RandomGenerator
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
RandomGenerator::RandomGenerator() : rootGenerator(std::time(NULL)) {}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// getInstance
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
RandomGenerator &RandomGenerator::getInstance() {
  static RandomGenerator instance;
  return instance;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// generateUniform
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
double RandomGenerator::generateUniform(double minimumValue, double maximumValue) {
  return rootGenerator.Uniform(minimumValue, maximumValue);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// generateGaussian
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
double RandomGenerator::generateGaussian(double mean, double sigma) {
  return rootGenerator.Gaus(mean, sigma);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// generateLongitude
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
double RandomGenerator::generateLongitude(double minimumAngle, double maximumAngle) {
  return rootGenerator.Uniform(minimumAngle, maximumAngle);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// generateColatitude
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
double RandomGenerator::generateColatitude(double minimumAngle, double maximumAngle) {
  // Extraction of theta
  const double uniform_theta = rootGenerator.Uniform(0., 1.);

  return 2. * asin(sqrt(uniform_theta * (1 - cos(maximumAngle)) / 2.)) - 2. * asin(sqrt(uniform_theta * (1 - cos(minimumAngle)) / 2.));
}