#include <iostream>
#include <getopt.h>

#include "TTree.h"
#include "ExRootTreeReader.h"
#include "Common.h"
#include "TClonesArray.h"
#include "TH1F.h"
#include "TChain.h"

using namespace std;

int main(int argc, char **argv) {
    char *eventsFileNames[50];
    char *jetWeightsFileName = NULL;
    char *eventWeightsFileName = NULL;
    char *histogramFileName = NULL;
    int nEvents = 0;
    int nEventsFiles = 0;
    int c;
    while(1) {
        int option_index = 0;
        static struct option long_options[] = {
            {"eventsFile", required_argument, 0, 'a'},
            {"jetClassifierWeights", required_argument, 0, 'b'},
            {"eventClassifierWeights", required_argument, 0, 'c'},
            {"nEvents", required_argument, 0, 'd'},
            {"histogramFile", required_argument, 0, 'e'},
        };
        c = getopt_long(argc, argv, "abcde:", long_options, &option_index);
        if (c==-1)
            break;
        switch(c) {
            case 'a':
                eventsFileNames[nEventsFiles] = optarg;
                nEventsFiles++;
                break;
            case 'b':
                jetWeightsFileName = optarg; 
                break;
            case 'c':
                eventWeightsFileName = optarg;
                break;
            case 'd':
                nEvents = atoi(optarg);
                break;
            case 'e':
                histogramFileName = optarg;
                break;
        }
    }
    
    JetClassifier *jetClassifier = new JetClassifier(jetWeightsFileName);
    EventClassifier *eventClassifier = new EventClassifier(eventWeightsFileName);
    TChain *chain = new TChain("Delphes");
    for (int i = 0; i < nEventsFiles; i++) {
        chain->Add(eventsFileNames[i]);
    }
    ExRootTreeReader *reader = new ExRootTreeReader(chain);

    TClonesArray *jetBranch = (TClonesArray*)reader->UseBranch("Jet");
    TClonesArray *electronBranch = (TClonesArray*)reader->UseBranch("Electron");
    TClonesArray *muonBranch = (TClonesArray*)reader->UseBranch("Muon");
    TClonesArray *ETBranch = (TClonesArray*)reader->UseBranch("MissingET");

    TFile *histogramFile = new TFile(histogramFileName, "RECREATE");
    TH1F *ww_mass_hist = new TH1F(WW_MASS_HISTOGRAM_NAME, "WW Invariant Mass", 100, 0, 2500);
    int nClassifierFails = 0;
    int nGoodEvents = 0;

    TH1F *TMVAResponseHist = new TH1F("TMVAResponse", "TMVA Response", 100, -2, 2);

    for (int i = 0; i < nEvents; i++) {
        if (i % 10 == 0) std::cerr << "Number of events: " << i << "\n";
        reader->ReadEntry(i);
        struct WWScatteringComponents *event = new WWScatteringComponents(jetClassifier, 
                electronBranch, muonBranch, jetBranch, ETBranch);
        
        //Can't proceed unless all final-state particles were found
        if (!event->isGoodEvent) {
            continue;
        }
        nGoodEvents++;

        //Decide whether this is a background event (W+jets or TTbar) or signal event (ww scattering) 
        //using the event classifier
        Double_t score = eventClassifier->ScoreEvent(event);
        TMVAResponseHist->Fill(score);
        if (score < EVENT_MVA_CUTOFF) {
            std::cerr << "Event didn't pass classifier.\n";
            nClassifierFails++;
            continue;
        }

        TLorentzVector *WW = ReconstructWW(event->lepton, event->hadronicJet, event->missingET);
        if (!WW) continue;
        ww_mass_hist->Fill(WW->M());
    }
    ww_mass_hist->Write();
    TMVAResponseHist->Write();
    
    histogramFile->Close();
    std::cerr << "Out of " << nGoodEvents << " good events, " << nClassifierFails << " failed the classifier.\n";
}
