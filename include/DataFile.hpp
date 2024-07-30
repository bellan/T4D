#pragma once

#include "MeasuresAndStates.hpp"

#include <TFile.h>
#include <TTree.h>
#include <vector>

class DataFile {
public:
    DataFile(const char* fileName="../data/GeneratedData.root", const char* treeName="DataTree", bool exist=false);
    ~DataFile();

    void SaveSingleMeasure(Measurement measure);
    void SaveMultipleMeasures(std::vector<Measurement> measuremets);

    std::vector<Measurement> readMeasures();

private:
    double tBuffer;
    double xBuffer;
    double yBuffer;
    int idBuffer;

    const char* treeName;
    bool writable;

    TFile* rootFile;
    TTree* dataTree;
};
