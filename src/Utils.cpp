#include "Utils.hpp"

#include "Detector.hpp"
#include "MeasuresAndStates.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>

void Utils::printMatrix(TMatrixD matrix) {
  std::cout << std::scientific << std::setprecision(2);
  const int nrows = matrix.GetNrows();
  const int ncols = matrix.GetNcols();
  for (int i = 0; i < nrows; i++) {
    std::cout << (i == 0 ? "[" : " ");
    for (int j = 0; j < ncols; j++) {
      std::cout << matrix(i, j) << ", ";
    }
    std::cout << (i == nrows - 1 ? "]" : "") << std::endl;
  }
}

// TODO: Implement the real definition in the header (increasing the spees
// should be enough) and uncomment the line in the code. Othewise change the
// description.
std::vector<std::vector<Measurement>> Utils::separateMeasuresInParticles(
    const std::vector<Measurement> &allMeasures) {
  if (allMeasures.empty())
    throw std::invalid_argument("No measures given");

  // std::sort(allMeasures.begin(), allMeasures.end(), [](Measurement a,
  // Measurement b){return a.t < b.t;});

  std::vector<std::vector<Measurement>> singleParticleMeasuresVectors;

  for (Measurement measure : allMeasures) {
    if (singleParticleMeasuresVectors.empty() ||
        measure.detectorID <
            singleParticleMeasuresVectors.back().back().detectorID) {
      singleParticleMeasuresVectors.push_back(std::vector<Measurement>());
    }
    singleParticleMeasuresVectors.back().push_back(measure);
  }

  return singleParticleMeasuresVectors;
}

std::vector<Measurement> Utils::concatenateMeasures(
    const std::vector<std::vector<Measurement>> &separateMeasures) {
  std::vector<Measurement> allMeasures;
  for (const std::vector<Measurement> &singleVector : separateMeasures)
    allMeasures.insert(allMeasures.end(), singleVector.begin(),
                       singleVector.end());
  return allMeasures;
}

void Utils::saveDataToCSV(
    const std::vector<Detector> &detectors,
    const std::vector<std::vector<ParticleState>> &theoreticalStates,
    const std::vector<std::vector<ParticleState>> &realStates,
    const std::vector<std::vector<Measurement>> &measures,
    const std::vector<std::vector<MatrixStateEstimate>> &predictedStates,
    const std::vector<std::vector<MatrixStateEstimate>> &filteredStates,
    const std::vector<std::vector<MatrixStateEstimate>> &smoothedStates) {

  const bool particleLengthCheck =
      theoreticalStates.size() == realStates.size() &&
      theoreticalStates.size() == predictedStates.size() &&
      theoreticalStates.size() == filteredStates.size() &&
      theoreticalStates.size() == smoothedStates.size() &&
      theoreticalStates.size() == measures.size();
  if (!particleLengthCheck)
    throw std::invalid_argument("saveDataToCSV: vectors of different size");

  // Particles loop
  for (int j = 0; j < (int)theoreticalStates.size(); j++) {
    std::string filename("../results/Particle ");
    filename += std::to_string(j);
    filename += ".csv";
    std::ofstream csvFile;
    csvFile.open(filename);
    csvFile << "z,theoretical,,,,,,real,,,,,,measured,,,predicted,,,,,,,,,,,,"
               "filtered,,,,,,"
               ",,,,,,smoothed,,,,,,,,,,,\n";
    csvFile << "z,t,x,y,speeed,xz,yz,t,x,y,speeed,xz,yz,t,x,y,t,st,x,sx,y,sy,"
               "speeed,sspeed,xz,sxz,yz,"
               "syz,t,st,x,sx,y,sy,speeed,sspeed,xz,sxz,yz,syz,t,st,x,sx,y,sy,"
               "speeed,sspeed,xz,sxz,yz,syz\n";
    for (int i = 0; i < (int)theoreticalStates[j].size(); i++) {
      Measurement meas;
      if (i == 0) {
        csvFile << "0.,";
        meas = Measurement{0, 0, 0, 1};
      } else if (i > (int)measures[j].size()) {
        break;
      } else {
        csvFile << detectors[i - 1].getBottmLeftPosition().z() << ",";
        meas = measures[j][i - 1];
      }

      ParticleState the = theoreticalStates[j][i];
      ParticleState rea = realStates[j][i];
      MatrixStateEstimate pre = predictedStates[j][i];
      MatrixStateEstimate fil = filteredStates[j][i];
      MatrixStateEstimate smo = smoothedStates[j][i];

      csvFile << the.position.T() << "," << the.position.X() << ","
              << the.position.Y() << "," << 1. / the.velocity.Z() << ","
              << the.velocity.X() / the.velocity.Z() << ","
              << the.velocity.Y() / the.velocity.Z() << ",";

      csvFile << rea.position.T() << "," << rea.position.X() << ","
              << rea.position.Y() << "," << 1. / rea.velocity.Z() << ","
              << rea.velocity.X() / rea.velocity.Z() << ","
              << rea.velocity.Y() / rea.velocity.Z() << ",";

      csvFile << meas.t << "," << meas.x << "," << meas.y << ",";

      csvFile << pre.value(0, 0) << "," << sqrt(pre.uncertainty(0, 0)) << ","
              << pre.value(1, 0) << "," << sqrt(pre.uncertainty(1, 1)) << ","
              << pre.value(2, 0) << "," << sqrt(pre.uncertainty(2, 2)) << ","
              << pre.value(3, 0) << "," << sqrt(pre.uncertainty(3, 3)) << ","
              << pre.value(4, 0) << "," << sqrt(pre.uncertainty(4, 4)) << ","
              << pre.value(5, 0) << "," << sqrt(pre.uncertainty(5, 5)) << ",";

      csvFile << fil.value(0, 0) << "," << sqrt(fil.uncertainty(0, 0)) << ","
              << fil.value(1, 0) << "," << sqrt(fil.uncertainty(1, 1)) << ","
              << fil.value(2, 0) << "," << sqrt(fil.uncertainty(2, 2)) << ","
              << fil.value(3, 0) << "," << sqrt(fil.uncertainty(3, 3)) << ","
              << fil.value(4, 0) << "," << sqrt(fil.uncertainty(4, 4)) << ","
              << fil.value(5, 0) << "," << sqrt(fil.uncertainty(5, 5)) << ",";

      csvFile << smo.value(0, 0) << "," << sqrt(smo.uncertainty(0, 0)) << ","
              << smo.value(1, 0) << "," << sqrt(smo.uncertainty(1, 1)) << ","
              << smo.value(2, 0) << "," << sqrt(smo.uncertainty(2, 2)) << ","
              << smo.value(3, 0) << "," << sqrt(smo.uncertainty(3, 3)) << ","
              << smo.value(4, 0) << "," << sqrt(smo.uncertainty(4, 4)) << ","
              << smo.value(5, 0) << "," << sqrt(smo.uncertainty(5, 5)) << "\n";
    }
    csvFile.close();
  }
}

void Utils::saveDataToCSV(
    const std::vector<Detector> &detectors,
    const std::vector<std::vector<ParticleState>> &realStates,
    const std::vector<std::vector<Measurement>> &measures,
    const std::vector<std::vector<MatrixStateEstimate>> &smoothedStates) {

  const bool particleLengthCheck = realStates.size() == smoothedStates.size() &&
                                   realStates.size() == measures.size();
  if (!particleLengthCheck)
    throw std::invalid_argument("saveDataToCSV: vectors of different size");

  // Particles loop
  for (int j = 0; j < (int)realStates.size(); j++) {
    std::string filename("../results/Detector test part ");
    filename += std::to_string(j);
    filename += ".csv";
    std::ofstream csvFile;
    csvFile.open(filename);
    csvFile << "z,real,,,,,,measured,,,smoothed,,,,,,,,,,,,\n";
    csvFile << "z,t,x,y,speeed,xz,yz,t,x,y,t,st,x,sx,y,sy,"
               "speeed,sspeed,xz,sxz,yz,syz\n";
    for (int i = 0; i < (int)realStates[j].size(); i++) {
      Measurement meas;
      if (i == 0) {
        csvFile << "0.,";
        meas = Measurement{0, 0, 0, 1};
      } else if (i > (int)measures[j].size()) {
        break;
      } else {
        csvFile << detectors[i - 1].getBottmLeftPosition().z() << ",";
        meas = measures[j][i - 1];
      }

      ParticleState rea = realStates[j][i];
      MatrixStateEstimate smo = smoothedStates[j][i];

      csvFile << rea.position.T() << "," << rea.position.X() << ","
              << rea.position.Y() << "," << 1. / rea.velocity.Z() << ","
              << rea.velocity.X() / rea.velocity.Z() << ","
              << rea.velocity.Y() / rea.velocity.Z() << ",";

      csvFile << meas.t << "," << meas.x << "," << meas.y << ",";

      csvFile << smo.value(0, 0) << "," << sqrt(smo.uncertainty(0, 0)) << ","
              << smo.value(1, 0) << "," << sqrt(smo.uncertainty(1, 1)) << ","
              << smo.value(2, 0) << "," << sqrt(smo.uncertainty(2, 2)) << ","
              << smo.value(3, 0) << "," << sqrt(smo.uncertainty(3, 3)) << ","
              << smo.value(4, 0) << "," << sqrt(smo.uncertainty(4, 4)) << ","
              << smo.value(5, 0) << "," << sqrt(smo.uncertainty(5, 5)) << "\n";
    }
    csvFile.close();
  }
}

void Utils::printTime(float btime, float etime){
  std::cout << "\n Execution time: " << (int)((etime - btime)/3600) << " h " 
            << (((int)(etime - btime)%3600)/60) << " m " 
            << etime - btime - (int)((etime - btime)/3600)*3600 - (((int)(etime - btime)%3600)/60)*60 << " s." << std::endl;
}
