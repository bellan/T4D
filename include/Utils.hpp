#pragma once

// Header files needed
#include <TMatrixD.h>

// Custom classes

// Namespaces
namespace Utils {

/**
 * Print a TMatrixD to stdout.
 *
 * @param matrix the matrix to be printed.
 */
void printMatrix(TMatrixD matrix);


/**
 * Print elapsed time in human readable format
 * 
 * @param btime begin time.
 * @param etime end time.
 */
void printTime(float btime, float etime);

}
