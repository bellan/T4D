#pragma once

#include "MeasuresAndStates.hpp"

#include <TFile.h>
#include <TTree.h>
#include <vector>

class DataFile {
public:
  /**
   * The default constructor
   *
   * @param fileName the name of the file
   * @param treeName the name of the Tree
   */
  DataFile(const char *fileName = "../data/GeneratedData.root",
           const char *treeName = "DataTree", bool exist = false);

  /**
   * The destructor.
   *
   * Saves the tree in the file before closing.
   */
  ~DataFile();

  /**
   * Save a single measuremet to the file.
   *
   * @param measure the measure to be saved.
   */
  void SaveSingleMeasure(Measurement measure);

  /**
   * Save a vector of measuremets to the file
   *
   * @param measures the vector of measures to be saved
   */
  void SaveMultipleMeasures(const std::vector<Measurement> &measuremets);

  /**
   * Read measures from the tree
   *
   * @return a vector containg all the measurements in the file
   */
  std::vector<Measurement> readMeasures();

private:
  double tBuffer;
  double xBuffer;
  double yBuffer;
  int idBuffer;

  const char *treeName;
  bool writable;

  TFile *rootFile;
  TTree *dataTree;
};
