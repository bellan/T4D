#pragma once

#include <TMatrixD.h>

using namespace std;

/**
 * The matrix rapresentation of the estimated state.
 *
 * It contains the value and the uncertainty of the estimate.
 */
struct MatrixStateEstimate {
    public:
      TMatrixD value;
      TMatrixD uncertainty;
      int detectorID = -2;
    };
