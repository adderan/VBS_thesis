#include <iostream>
#include <getopt.h>

#include "TTree.h"
#include "ExRootTreeReader.h"
#include "Common.h"
#include "TClonesArray.h"
#include "TH1F.h"

using namespace std;

int main(int argc, char **argv) {
    char *eventsFileName = NULL;
    char *jetWeightsFileName = NULL;
    char *eventWeightsFileName = NULL;
    char *histogramFileName = NULL;
    int c;
    while(1) {
        int option_index = 0;
        static struct option long_options[] = {
            {"events", required_argument, 0, 'a'},
            {"jetClassifierWeights", required_argument, 0, 'b'},
            {"eventClassifierWeights", required_argument, 0, 'c'},
            {"histogramFile", required_argument, 0, 'd'},
        };
        c = getopt_long(argc, argv, "abcd:", long_options, &option_index);
        if (c==-1)
            break;
        switch(c) {
            case 'a':
                eventsFileName = optarg;
                break;
            case 'b':
                jetWeightsFileName = optarg; 
                break;
            case 'c':
                eventWeightsFileName = optarg;
                break;
            case 'd':
                histogramFileName = optarg;
                break;
        }
    }
    cerr << "Using event weights: " << eventWeightsFileName << "\n";
    
    JetClassifier *jetClassifier = new JetClassifier(jetWeightsFileName);
    TFile *file = new TFile(eventsFileName);
    TTree *tree = (TTree*)file->Get("Delphes");
    ExRootTreeReader *reader = new ExRootTreeReader(tree);

    TClonesArray *jetBranch = (TClonesArray*)reader->UseBranch("Jet");
    TClonesArray *electronBranch = (TClonesArray*)reader->UseBranch("Electron");
    TClonesArray *muonBranch = (TClonesArray*)reader->UseBranch("Muon");
    TClonesArray *ETBranch = (TClonesArray*)reader->UseBranch("MissingET");

    TFile *histogramFile = new TFile(histogramFileName, "UPDATE");
    TH1F *ww_mass_hist = (TH1F*)histogramFile->Get(WW_MASS_HISTOGRAM_NAME);
    if (!ww_mass_hist) {
        ww_mass_hist = new TH1F(WW_MASS_HISTOGRAM_NAME, "WW Invariant Mass", 100, 0, 300);
    }

    int nEvents = reader->GetEntries();
    for (int i = 0; i < nEvents; i++) {
        reader->ReadEntry(i);
        Jet *posTagJet = NULL;
        Jet *negTagJet = NULL;
        FindTagJetPair(jetClassifier, jetBranch, &posTagJet, &negTagJet);

        Electron *electron;
        Muon *muon;
        bool found_lepton = FindLepton(electronBranch, muonBranch, &electron, &muon);

        Jet *hadronicJet = FindHadronicJet(jetBranch);

        if (!(posTagJet && negTagJet && found_lepton && hadronicJet)) {
            continue;
        }
        MissingET *missingET = (MissingET*)ETBranch->At(0);
        TLorentzVector *WWVector = ReconstructWW(electron, muon, hadronicJet, missingET);
        if (!WWVector) continue;
        ww_mass_hist->Fill(WWVector->M());
    }
    ww_mass_hist->Write();
    
    histogramFile->Close();
}
