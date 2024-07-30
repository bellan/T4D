#pragma once

#include <TRandom1.h>

class RandomGenerator {
public:
    static RandomGenerator& getInstance();

    RandomGenerator(const RandomGenerator&) = delete;
    RandomGenerator(const RandomGenerator&&) = delete;
    RandomGenerator operator = (const RandomGenerator&) = delete;
    RandomGenerator operator = (const RandomGenerator&&) = delete;

    double generateUniform(double minimumValue, double maximumuValue);
    double generateGaussian(double mean=0., double sigma=1.);
    double generateLongitude(double minimumAngle, double maximumAngle);
    double generateColatitude(double minimumAngle, double maximumAngle);

private:
    RandomGenerator();

    TRandom1 rootGenerator;
};
