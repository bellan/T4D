#pragma once

#include "Detector.hpp"

#include <TFile.h>
#include <TTree.h>
#include <vector>

// TODO: Make also readonly (now it rewrites every time)
class DataFile {
public:
    DataFile(const char* fileName="../data/GeneratedData.root", const char* treeName="DataTree");
    ~DataFile();

    void SaveSingleMeasure(Measurement measure);
    void SaveMultipleMeasures(std::vector<Measurement> measuremets);

    std::vector<Measurement> readMeasures();

private:
    std::unique_ptr<TFile> rootFile;
    const char* treeName;

    double tBuffer;
    double xBuffer;
    double yBuffer;
    int idBuffer;

    TTree dataTree;
};
