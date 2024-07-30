#include "RandomGenerator.hpp"

#include <cstddef>
#include <ctime>

/**
 * The default constructor.
 *
 * Generates the root object.
 */
RandomGenerator::RandomGenerator() : rootGenerator(std::time(NULL)) {}

/**
 * Static method for accessing the singleton instance.
 *
 * @return the reference to the instance of the random generator (singletor)
 */
RandomGenerator& RandomGenerator::getInstance() {
    static RandomGenerator instance;
    return instance;
}

double RandomGenerator::generateUniform(double minimumValue, double maximumValue) {
    return rootGenerator.Uniform(minimumValue, maximumValue);
}

double RandomGenerator::generateGaussian(double mean, double sigma) {
    return rootGenerator.Gaus(mean, sigma);
}

double RandomGenerator::generateLongitude(double minimumAngle, double maximumAngle) {
    return rootGenerator.Uniform(minimumAngle, maximumAngle);
}

double RandomGenerator::generateColatitude(double minimumAngle, double maximumAngle) {
    const double uniform_theta = rootGenerator.Uniform(0.,1.);
    return 2.*asin(sqrt(uniform_theta*(1-cos(maximumAngle))/2.)) - 2.*asin(sqrt(uniform_theta*(1-cos(minimumAngle))/2.));
}
