#include "ResultFile.hpp"
#include "MeasuresAndStates.hpp"

#include <TFile.h>
#include <TTree.h>
#include <cmath>

ResultFile::ResultFile(const char *fileName, const char *treeName) {
  rootFile = TFile::Open(fileName, "RECREATE");
  dataTree = new TTree(treeName, treeName);
  double *pointers[] = {
      &zBuffer,   &ttBuffer,   &txBuffer,  &tyBuffer,   &tvBuffer, &txzBuffer,
      &tyzBuffer, &rtBuffer,   &rxBuffer,  &ryBuffer,   &rvBuffer, &rxzBuffer,
      &ryzBuffer, &mtBuffer,   &mxBuffer,  &myBuffer,   &ptBuffer, &pstBuffer,
      &pxBuffer,  &psxBuffer,  &pyBuffer,  &psyBuffer,  &pvBuffer, &psvBuffer,
      &pxzBuffer, &psxzBuffer, &pyzBuffer, &psyzBuffer, &ftBuffer, &fstBuffer,
      &fxBuffer,  &fsxBuffer,  &fyBuffer,  &fsyBuffer,  &fvBuffer, &fsvBuffer,
      &fxzBuffer, &fsxzBuffer, &fyzBuffer, &fsyzBuffer, &stBuffer, &sstBuffer,
      &sxBuffer,  &ssxBuffer,  &syBuffer,  &ssyBuffer,  &svBuffer, &ssvBuffer,
      &sxzBuffer, &ssxzBuffer, &syzBuffer, &ssyzBuffer};
  const char *names[] = {
      "z",    "tt",   "tx",  "ty",   "tv",   "txz", "tyz", "rt",   "rx",
      "ry",   "rv",   "rxz", "ryz",  "mt",   "mx",  "my",  "pt",   "pst",
      "px",   "psx",  "py",  "psy",  "pv",   "psv", "pxz", "psxz", "pyz",
      "psyz", "ft",   "fst", "fx",   "fsx",  "fy",  "fsy", "fv",   "fsv",
      "fxz",  "fsxz", "fyz", "fsyz", "st",   "sst", "sx",  "ssx",  "sy",
      "ssy",  "sv",   "ssv", "sxz",  "ssxz", "syz", "ssyz"};

  for (int i = 0; i < 52; i++) {
    dataTree->Branch(names[i], pointers[i]);
  }
}

ResultFile::~ResultFile() {
  rootFile->WriteObject(dataTree, treeName);
  rootFile->Close();
  delete rootFile;
  /* NOTE: There is a memory leak here since I don't delete the TTree, however i
   * get an error if I do. Since there is only one instance, and it gets cleared
   * a few instruction later I can live with that.
   */
}

void ResultFile::SaveSingleValue(double z, ParticleState theoreticalState,
                                 ParticleState realState, Measurement measure,
                                 MatrixStateEstimate predictedState,
                                 MatrixStateEstimate filteredState,
                                 MatrixStateEstimate smoothedState) {
  zBuffer = z;

  ttBuffer = theoreticalState.position.T();
  txBuffer = theoreticalState.position.X();
  tyBuffer = theoreticalState.position.Y();
  tvBuffer = 1. / theoreticalState.velocity.Z();
  txzBuffer = theoreticalState.velocity.X() / theoreticalState.velocity.Z();
  tyzBuffer = theoreticalState.velocity.Y() / theoreticalState.velocity.Z();

  rtBuffer = realState.position.T();
  rxBuffer = realState.position.X();
  ryBuffer = realState.position.Y();
  rvBuffer = 1. / realState.velocity.Z();
  rxzBuffer = realState.velocity.X() / realState.velocity.Z();
  ryzBuffer = realState.velocity.Y() / realState.velocity.Z();

  mtBuffer = measure.t;
  mxBuffer = measure.x;
  myBuffer = measure.y;

  ptBuffer = predictedState.value(0, 0);
  pxBuffer = predictedState.value(1, 0);
  pyBuffer = predictedState.value(2, 0);
  pvBuffer = predictedState.value(3, 0);
  pxzBuffer = predictedState.value(4, 0);
  pyzBuffer = predictedState.value(5, 0);
  pstBuffer = sqrt(predictedState.uncertainty(0, 0));
  psxBuffer = sqrt(predictedState.uncertainty(1, 1));
  psyBuffer = sqrt(predictedState.uncertainty(2, 2));
  psvBuffer = sqrt(predictedState.uncertainty(3, 3));
  psxzBuffer = sqrt(predictedState.uncertainty(4, 4));
  psyzBuffer = sqrt(predictedState.uncertainty(5, 5));

  ftBuffer = filteredState.value(0, 0);
  fxBuffer = filteredState.value(1, 0);
  fyBuffer = filteredState.value(2, 0);
  fvBuffer = filteredState.value(3, 0);
  fxzBuffer = filteredState.value(4, 0);
  fyzBuffer = filteredState.value(5, 0);
  fstBuffer = sqrt(filteredState.uncertainty(0, 0));
  fsxBuffer = sqrt(filteredState.uncertainty(1, 1));
  fsyBuffer = sqrt(filteredState.uncertainty(2, 2));
  fsvBuffer = sqrt(filteredState.uncertainty(3, 3));
  fsxzBuffer = sqrt(filteredState.uncertainty(4, 4));
  fsyzBuffer = sqrt(filteredState.uncertainty(5, 5));

  stBuffer = smoothedState.value(0, 0);
  sxBuffer = smoothedState.value(1, 0);
  syBuffer = smoothedState.value(2, 0);
  svBuffer = smoothedState.value(3, 0);
  sxzBuffer = smoothedState.value(4, 0);
  syzBuffer = smoothedState.value(5, 0);
  sstBuffer = sqrt(smoothedState.uncertainty(0, 0));
  ssxBuffer = sqrt(smoothedState.uncertainty(1, 1));
  ssyBuffer = sqrt(smoothedState.uncertainty(2, 2));
  ssvBuffer = sqrt(smoothedState.uncertainty(3, 3));
  ssxzBuffer = sqrt(smoothedState.uncertainty(4, 4));
  ssyzBuffer = sqrt(smoothedState.uncertainty(5, 5));

  dataTree->Fill();
}

void ResultFile::SaveMultipleValues(
    const std::vector<Detector> &detectors,
    const std::vector<ParticleState> &theoreticalStates,
    const std::vector<ParticleState> &realStates,
    const std::vector<Measurement> &measures,
    const std::vector<MatrixStateEstimate> &predictedStates,
    const std::vector<MatrixStateEstimate> &filteredStates,
    const std::vector<MatrixStateEstimate> &smoothedStates) {
  for (int i = 0; i < (int)smoothedStates.size(); i++) {
    if (i == 0) {
      const Measurement measure{NAN, NAN, NAN, -1};
      SaveSingleValue(0., theoreticalStates[i], realStates[i], measure,
                      predictedStates[i], filteredStates[i], smoothedStates[i]);
      continue;
    }
    SaveSingleValue(detectors[i - 1].getBottmLeftPosition().Z(),
                    theoreticalStates[i], realStates[i], measures[i - 1],
                    predictedStates[i], filteredStates[i], smoothedStates[i]);
  }
}
