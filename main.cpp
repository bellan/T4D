#include "PhysicalParameters.hpp"
#include "Simulation.hpp"

int main() {

  auto simulation = Simulation();
  simulation.runSimulation(NUMBER_OF_PARTICLES);
  /*simulation.testDetector(NUMBER_OF_PARTICLES, 5);*/
  /*simulation.testDetector(1, 5);*/

  return 0;
}
