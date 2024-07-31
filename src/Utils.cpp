#include "Utils.hpp"

#include "Detector.hpp"
#include "MeasuresAndStates.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>

void printMatrix(TMatrixD matrix) {
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

// TODO: document
void saveDataToCSV(
    std::vector<Detector> detectors,
    std::vector<std::vector<ParticleState>> generatedStates,
    std::vector<std::vector<MatrixStateEstimate>> filteredStates,
    std::vector<std::vector<MatrixStateEstimate>> smoothedStates,
    std::vector<std::vector<MatrixStateEstimate>> predictedStates) {
  if (filteredStates.size() != generatedStates.size() ||
      filteredStates.size() != smoothedStates.size() ||
      smoothedStates.size() != predictedStates.size())
    std::cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" << std::endl;
  for (int j = 0; j < (int)generatedStates.size(); j++) {
    std::string filename("../data/Particle ");
    filename += std::to_string(j);
    filename += ".csv";
    std::ofstream csvFile;
    csvFile.open(filename);
    csvFile << "z,generated,,,,,,predicted,,,,,,,,,,,,filtered,,,,,,,,,,,,"
               "smoothed,,,,,,,,,,,\n";
    csvFile << "z,t,x,y,speeed,xz,yz,t,st,x,sx,y,sy,speeed,sspeed,xz,sxz,yz,"
               "syz,t,st,x,sx,y,sy,speeed,sspeed,xz,sxz,yz,syz,t,st,x,sx,y,sy,"
               "speeed,sspeed,xz,sxz,yz,syz\n";
    for (int i = 0; i < (int)generatedStates[j].size(); i++) {
      if (i == 0)
        csvFile << "0.,";
      else
        csvFile << detectors[i - 1].getBottmLeftPosition().z() << ",";

      ParticleState gen = generatedStates[j][i];
      MatrixStateEstimate pre = predictedStates[j][i];
      MatrixStateEstimate fil = filteredStates[j][i];
      MatrixStateEstimate smo = smoothedStates[j][i];

      csvFile << gen.position.T() << ",";
      csvFile << gen.position.X() << ",";
      csvFile << gen.position.Y() << ",";
      csvFile << 1. / gen.velocity.Z() << ",";
      csvFile << gen.velocity.X() / gen.velocity.Z() << ",";
      csvFile << gen.velocity.Y() / gen.velocity.Z() << ",";

      csvFile << pre.value(0, 0) << ",";
      csvFile << sqrt(pre.uncertainty(0, 0)) << ",";
      csvFile << pre.value(1, 0) << ",";
      csvFile << sqrt(pre.uncertainty(1, 1)) << ",";
      csvFile << pre.value(2, 0) << ",";
      csvFile << sqrt(pre.uncertainty(2, 2)) << ",";
      csvFile << pre.value(3, 0) << ",";
      csvFile << sqrt(pre.uncertainty(3, 3)) << ",";
      csvFile << pre.value(4, 0) << ",";
      csvFile << sqrt(pre.uncertainty(4, 4)) << ",";
      csvFile << pre.value(5, 0) << ",";
      csvFile << sqrt(pre.uncertainty(5, 5)) << ",";

      csvFile << fil.value(0, 0) << ",";
      csvFile << sqrt(fil.uncertainty(0, 0)) << ",";
      csvFile << fil.value(1, 0) << ",";
      csvFile << sqrt(fil.uncertainty(1, 1)) << ",";
      csvFile << fil.value(2, 0) << ",";
      csvFile << sqrt(fil.uncertainty(2, 2)) << ",";
      csvFile << fil.value(3, 0) << ",";
      csvFile << sqrt(fil.uncertainty(3, 3)) << ",";
      csvFile << fil.value(4, 0) << ",";
      csvFile << sqrt(fil.uncertainty(4, 4)) << ",";
      csvFile << fil.value(5, 0) << ",";
      csvFile << sqrt(fil.uncertainty(5, 5)) << ",";

      csvFile << smo.value(0, 0) << ",";
      csvFile << sqrt(smo.uncertainty(0, 0)) << ",";
      csvFile << smo.value(1, 0) << ",";
      csvFile << sqrt(smo.uncertainty(1, 1)) << ",";
      csvFile << smo.value(2, 0) << ",";
      csvFile << sqrt(smo.uncertainty(2, 2)) << ",";
      csvFile << smo.value(3, 0) << ",";
      csvFile << sqrt(smo.uncertainty(3, 3)) << ",";
      csvFile << smo.value(4, 0) << ",";
      csvFile << sqrt(smo.uncertainty(4, 4)) << ",";
      csvFile << smo.value(5, 0) << ",";
      csvFile << sqrt(smo.uncertainty(5, 5)) << "\n";
    }
    csvFile.close();
  }
}
