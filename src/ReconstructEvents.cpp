#include <iostream>

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
    char *outputFileName = NULL;
    int c;
    while(1) {
        int option_index = 0;
        static struct option long_options[] = {
            {"events", required_argument, 0, 'a'},
            {"jetClassifierWeights", required_argument, 0, 'b'},
            {"eventClassifierWeights", required_argument, 0, 'c'},
            {"outputHistogram", required_argument, 0, 'd'},
        };
        c = getopt_long(argc, argv, "abcd:", long_options, &option_index);
        if (c==-1)
            break;
        switch(c) {
            case 'a':
                evetnsFileName = optarg;
                break;
            case 'b':
                jetWeightsFileName = optarg; 
                break;
            case 'c':
                eventWeightsFileName = optarg;
                break;
            case 'd':
                outputFileName = optarg;
                break;
        }
    }

    TFile *file = new TFile(outputFileName);
    TTree *tree = (TTree*)file->Get("Delphes");
    ExRootTreeReader *reader = new ExRootTreeReader(tree);

    TClonesArray *jetBranch = (TClonesArray*)reader->UseBranch("Jet");
    TClonesArray *electronBranch = (TClonesArray*)reader->UseBranch("Electron");
    TClonesArray *muonBranch = (TClonesArray*)reader->UseBranch("Muon");
    TClonesArray *ETBranch = (MissingET*)reader->UseBranch("MissingET");
    TH1F *ww_mass_hist = new TH1F();

    int nEvents = reader->GetEntries();
    for (int i = 0; i < nEvents; i++) {
        reader->ReadEntry(i);
        Jet *posTagJet = NULL;
        Jet *negTagJet = NULL;
        FindTagJets(jetBranch, &posTagJet, &negTagJet);

        Electron *electron;
        Muon *muon;
        bool found_lepton = FindLepton(electronBranch, muonBranch, &electron, &muon);

        Jet *hadronicJet = FindHadronicJet(jetBranch);

        if (!(posTagJet && negTagJet && found_lepton && hadronicJet)) {
            continue;
        }
        TLorentzVector *WWVector = ReconstructWW(electron, muon, hadronicJet);
        ww_mass_hist->Fill(WWVector->M());
    }
    TFile *outputFile = new TFile(outputFileName, "RECREATE");
    ww_mass_hist->Write();
    
    outputFile->Close();
}
