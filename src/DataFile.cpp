#include "DataFile.hpp"
#include "Detector.hpp"
#include <TFile.h>
#include <vector>

/**
 * The default constructor
 *
 * @param fileName the name of the file
 * @param treeName the name of the Tree
 */
DataFile::DataFile(const char* fileName, const char* treeName):
rootFile(TFile::Open(fileName, "RECREATE")),
tBuffer(0),
xBuffer(0),
yBuffer(0),
idBuffer(1),
treeName(treeName),
dataTree(treeName, treeName)
{
    dataTree.Branch("t", &tBuffer);
    dataTree.Branch("x", &xBuffer);
    dataTree.Branch("y", &yBuffer);
    dataTree.Branch("id", &idBuffer);
}

/**
 * The destructor
 *
 * Saves the tree in the file before closing.
 */
DataFile::~DataFile() {
    rootFile->WriteObject(&dataTree, treeName);
    rootFile->Close();
}

// TODO: Remove Printing
/**
 * Save a single measuremet to the file
 *
 * @param measure the measure to be saved.
 */
void DataFile::SaveSingleMeasure(Measurement measure) {
    std::cout<<measure.detectorID<<" " << measure.t << " " <<measure.x<<" " <<measure.y << std::endl;
    tBuffer = measure.t;
    xBuffer = measure.x;
    yBuffer = measure.y;
    idBuffer = measure.detectorID;
    dataTree.Fill();
}

// TODO: Remove printing
/**
 * Save a vector of measuremets to the file
 *
 * @param measures the vector of measures to be saved.
 */
void DataFile::SaveMultipleMeasures(std::vector<Measurement> measures) {
    for (const Measurement measure: measures) {
        std::cout<<measure.detectorID<<" " << measure.t << " " <<measure.x<<" " <<measure.y << std::endl;
        tBuffer = measure.t;
        xBuffer = measure.x;
        yBuffer = measure.y;
        idBuffer = measure.detectorID;
        dataTree.Fill();
    }
}
