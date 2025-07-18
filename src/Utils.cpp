// Header files needed
#include <iomanip>
#include <iostream>
#include <TMatrixD.h>

// Custom classes
#include "Utils.hpp"

// Namespaces
using namespace std;



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// printMatrix
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Utils::printMatrix(TMatrixD matrix) {
  // Printout settings
  cout << std::scientific << setprecision(2);

  // Matrix dimensions
  const int nrows = matrix.GetNrows();
  const int ncols = matrix.GetNcols();

  // Printout the matrix
  for (int i = 0; i < nrows; i++) {
    cout << (i == 0 ? "[" : " ");

    for (int j = 0; j < ncols; j++) {
      cout << matrix(i, j) << ", ";
    }

    cout << (i == nrows - 1 ? "]" : "") << endl;
  }
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// printTime
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Utils::printTime(float btime, float etime) {
  cout << "\n Execution time: " << (int)((etime - btime) / 3600) << " h " 
      << (((int)(etime - btime) % 3600) / 60) << " m "
      << etime - btime - (int)((etime - btime) / 3600) * 3600 - (((int)(etime - btime) % 3600) / 60) * 60 << " s." << endl;
}
