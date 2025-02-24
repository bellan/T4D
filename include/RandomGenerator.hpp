#pragma once

#include <TRandom3.h>

class RandomGenerator {
public:
  /**
   * Static method for accessing the singleton instance.
   *
   * @return the reference to the instance of the random generator (singletor)
   */
  static RandomGenerator &getInstance();

  RandomGenerator(const RandomGenerator &) = delete;
  RandomGenerator(const RandomGenerator &&) = delete;
  RandomGenerator operator=(const RandomGenerator &) = delete;
  RandomGenerator operator=(const RandomGenerator &&) = delete;

  /**
   * Generate a random number in a uniform distribution
   *
   * @param minimumValue the minimum of the range from which extract
   * @param maximumValue the maximum of the range from which extract
   * @return the random number generated
   */
  double generateUniform(double minimumValue, double maximumuValue);

  /**
   * Generate a random number in a gaussian distribution
   *
   * @param mean the mean of the gaussian distribution
   * @param sigma the standard deviation of the gaussian distribution
   * @return the random number generated
   */
  double generateGaussian(double mean = 0., double sigma = 1.);

  /**
   * Generate a random number corresponding to the longitude so that direction
   * is uniform in the selected area
   *
   * @param minimumAngle the minimum of the range from which extract
   * @param maximumValue the maximum of the range from which extract
   * @return the random number generated
   */
  double generateLongitude(double minimumAngle, double maximumAngle);

  /**
   * Generate a random number corresponding to the colatitude so that direction
   * is uniform in the selected area
   *
   * @param minimumAngle the minimum of the range from which extract
   * @param maximumValue the maximum of the range from which extract
   * @return the random number generated
   */
  double generateColatitude(double minimumAngle, double maximumAngle);

private:
  RandomGenerator();

  TRandom3 rootGenerator;
};
