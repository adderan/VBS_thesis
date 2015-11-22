#include <iostream>
#include <getopt.h>

#include "ExRootTreeReader.h"
#include "ExRootClasses.h"

#include "TTree.h"
#include "TFile.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TH1F.h"

#include "Common.h"

int main(int argc, char **argv) {
    char *eventsFileNames[50];
    int nEventsFiles = 0;
    char *histogramFileName;
    int c;
    while(1) {
        int option_index = 0;
        static struct option long_options[] = {
            {"events", required_argument, 0, 'a'},
            {"histogram", required_argument, 0, 'b'}
        };
        c = getopt_long(argc, argv, "ab:", long_options, &option_index);
        if (c==-1)
            break;
        switch(c) {
            case 'a':
                eventsFileNames[nEventsFiles] = optarg;
                nEventsFiles++;
                break;
            case 'b':
                histogramFileName = optarg;
                break;
        }
    }

    TChain *chain = new TChain("LHEF");
    for (int i = 0; i < nEventsFiles; i++) {
        chain->Add(eventsFileNames[i]);
    }
    ExRootTreeReader *reader = new ExRootTreeReader(chain);
    TClonesArray *particles = (TClonesArray*)reader->UseBranch("Particle");

    TFile *histogramFile = new TFile(histogramFileName, "RECREATE");
    TH1F *histogram = new TH1F("WWMass", "WW Invariant Mass", 100, 0, 800);

    int nEntries = reader->GetEntries();
    for (int i = 0; i < nEntries; i++) {
        reader->ReadEntry(i);
        TLorentzVector *lepton = NULL;
        TLorentzVector *neutrino = NULL;
        TLorentzVector *quark1 = NULL;
        TLorentzVector *quark2 = NULL;
        TLorentzVector *w1 = NULL;
        TLorentzVector *w2 = NULL;
        MatchPartonWWScatteringEvent(particles, &lepton, &neutrino, &quark1, &quark2, &w1, &w2);
        
        if (!(lepton && neutrino && quark1 && quark2 && w1 && w2)) {
            continue;
        }

        //apprximate missing ET
        TLorentzVector *met = new TLorentzVector();
        met->SetPtEtaPhiE(neutrino->Pt(), neutrino->Eta(), neutrino->Phi(), neutrino->Pt());
        TLorentzVector *reconstructedNeutrino = ReconstructNeutrinoAlternate(met, lepton);
        TLorentzVector *reconstructedWW = ReconstructWW(lepton, quark1, met);

        TLorentzVector *trueWW = new TLorentzVector(*w1 + *w2);
        if (!reconstructedNeutrino) continue;
        std::cerr << "Truth n pz: " << neutrino->Pz() << ", Rec. n pz: " 
            << reconstructedNeutrino->Pz() << " Truth ww: " << trueWW->M() << " rec. WW: " << reconstructedWW->M() << "\n";
        histogram->Fill(reconstructedWW->M());

    }
    histogram->Write();
    histogramFile->Close();

}

