#pragma once

#include "Detector.hpp"
#include "MeasuresAndStates.hpp"

#include <TFile.h>
#include <TTree.h>
#include <vector>

class ResultFile {
public:
  ResultFile(const char *fileName = "../data/ProcessedData.root",
             const char *treeName = "ResultTree");
  ~ResultFile();

  void SaveSingleValue(double z, ParticleState theoreticalState,
                       ParticleState realState, Measurement measure,
                       MatrixStateEstimate predictedState,
                       MatrixStateEstimate filteredState,
                       MatrixStateEstimate smoothedState);
  void SaveMultipleValues(
      const std::vector<Detector> &detectors,
      const std::vector<ParticleState> &theoreticalStates,
      const std::vector<ParticleState> &realStates,
      const std::vector<Measurement> &measures,
      const std::vector<MatrixStateEstimate> &predictedStates,
      const std::vector<MatrixStateEstimate> &filteredStates,
      const std::vector<MatrixStateEstimate> &smoothedStates);

private:
  double zBuffer;
  double ttBuffer, txBuffer, tyBuffer, tvBuffer, txzBuffer, tyzBuffer;
  double rtBuffer, rxBuffer, ryBuffer, rvBuffer, rxzBuffer, ryzBuffer;
  double mtBuffer, mxBuffer, myBuffer;
  double ptBuffer, pstBuffer, pxBuffer, psxBuffer, pyBuffer, psyBuffer,
      pvBuffer, psvBuffer, pxzBuffer, psxzBuffer, pyzBuffer, psyzBuffer;
  double ftBuffer, fstBuffer, fxBuffer, fsxBuffer, fyBuffer, fsyBuffer,
      fvBuffer, fsvBuffer, fxzBuffer, fsxzBuffer, fyzBuffer, fsyzBuffer;
  double stBuffer, sstBuffer, sxBuffer, ssxBuffer, syBuffer, ssyBuffer,
      svBuffer, ssvBuffer, sxzBuffer, ssxzBuffer, syzBuffer, ssyzBuffer;
 
  const char *treeName;

  TFile *rootFile;
  TTree *dataTree;
};
