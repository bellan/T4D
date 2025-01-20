// Interfaces
#include "PhysicalParameters.hpp"
#include "Simulation.hpp"
#include "Utils.hpp"

// Other libraries
#include <ctime> 

// Namespaces
using namespace std;
using namespace Utils;

int main() {
  // --- Execution time
  cout << "\n--------------------------------------------------------------------------" << endl;
  auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  cout << " Beginning simulation at: " << ctime(&now) << endl;
  float begintime = ((float)clock())/CLOCKS_PER_SEC;

  // --- Simulation
  auto simulation = Simulation();
  simulation.runSimulation(NUMBER_OF_PARTICLES);

  // --- Simulation of layers testing
  //simulation.testDetector(NUMBER_OF_PARTICLES, 5);
  //simulation.testDetector(1, 5);

  // --- Execution time
  now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  cout << "\n Finished analysis at: " << ctime(&now);
  float endtime = ((float)clock())/CLOCKS_PER_SEC;
  Utils::printTime(begintime, endtime);
  cout << "\n--------------------------------------------------------------------------" << endl;

  return 0;
}
