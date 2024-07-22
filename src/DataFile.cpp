#include "DataFile.hpp"
#include "Detector.hpp"

#include <TFile.h>
#include <TTree.h>
#include <stdexcept>
#include <vector>

/**
 * The default constructor
 *
 * @param fileName the name of the file
 * @param treeName the name of the Tree
 */
DataFile::DataFile(const char* fileName, const char* treeName, bool exists):
tBuffer(0),
xBuffer(0),
yBuffer(0),
idBuffer(1),
treeName(treeName),
writable(!exists)
{
    rootFile = exists ? TFile::Open(fileName) : TFile::Open(fileName, "RECREATE");
    dataTree = exists ? rootFile->Get<TTree>(treeName) : new TTree(treeName, treeName);
    if (exists) {
        dataTree->SetBranchAddress("t", &tBuffer);
        dataTree->SetBranchAddress("x", &xBuffer);
        dataTree->SetBranchAddress("y", &yBuffer);
        dataTree->SetBranchAddress("id", &idBuffer);
    } else {
        dataTree->Branch("t", &tBuffer);
        dataTree->Branch("x", &xBuffer);
        dataTree->Branch("y", &yBuffer);
        dataTree->Branch("id", &idBuffer);
    }
}

/**
 * The destructor
 *
 * Saves the tree in the file before closing.
 */
DataFile::~DataFile() {
    if (writable) rootFile->WriteObject(dataTree, treeName);
    rootFile->Close();
    delete rootFile;
    /* NOTE: There is a memory leak here since I don't delete the TTree, however i get an error if I do.
     *          Since there is only one instance, and it gets cleared a few instruction later I can live with that.
     */
}

/**
 * Save a single measuremet to the file
 *
 * @param measure the measure to be saved.
 */
void DataFile::SaveSingleMeasure(Measurement measure) {
    if (!writable) throw std::invalid_argument("You cannot write data to a readonly file");
    tBuffer = measure.t;
    xBuffer = measure.x;
    yBuffer = measure.y;
    idBuffer = measure.detectorID;
    dataTree->Fill();
}

/**
 * Save a vector of measuremets to the file
 *
 * @param measures the vector of measures to be saved.
 */
void DataFile::SaveMultipleMeasures(std::vector<Measurement> measures) {
    if (!writable) throw std::invalid_argument("You cannot write data to a readonly file");
    for (const Measurement measure: measures) {
        tBuffer = measure.t;
        xBuffer = measure.x;
        yBuffer = measure.y;
        idBuffer = measure.detectorID;
        dataTree->Fill();
    }
}

/**
 * Read measures from the tree
 *
 * @return a vector containg all the measurements in the file
 */
std::vector<Measurement> DataFile::readMeasures() {
    std::vector<Measurement> measures;
    for (int iEntry = 0; dataTree->LoadTree(iEntry) >= 0; ++iEntry) {
        dataTree->GetEntry(iEntry);

        // measures.push_back(Measurement{t, x, y, id});
        measures.push_back(Measurement{tBuffer, xBuffer, yBuffer, idBuffer});
        std::cout<<idBuffer<<" "<<tBuffer<<" "<<xBuffer<<" "<<yBuffer<<std::endl;
    }
    return measures;
}
