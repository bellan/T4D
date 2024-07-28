#include "Utils.hpp"

void printMatrix(TMatrixD matrix) {
    std::cout << std::scientific << std::setprecision(2);
    const int nrows = matrix.GetNrows();
    const int ncols = matrix.GetNcols();
    for (int i = 0; i < nrows; i++) {
        std::cout << (i == 0 ? "[" : " ");
        for (int j = 0; j < ncols; j++) {
            std::cout << matrix(i,j) << ", ";
        }
        std::cout << (i == nrows-1 ? "]" : "") << std::endl;
    }
}
