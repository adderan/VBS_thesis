#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>


#include "TMVA/Factory.h"
#include "ExRootTreeReader.h"
#include "ExRootTreeWriter.h"
#include "ExRootTreeBranch.h"

#include "Common.h"
#include "WWScatteringEvent.h"
#include "Classifiers.h"

using namespace std;

void makeTrainingEvents(JetClassifier *classifier, ExRootTreeReader *reader, ExRootTreeWriter *writer, int nEvents) {
    ExRootTreeBranch *eventBranch = writer->NewBranch("WWScatteringEvent", WWScatteringEvent::Class());


    TClonesArray *jetBranch = (TClonesArray*)reader->UseBranch("Jet");
    TClonesArray *muonBranch = (TClonesArray*)reader->UseBranch("Muon");
    TClonesArray *electronBranch = (TClonesArray*)reader->UseBranch("Electron");
    MissingET *missingET = (MissingET*)reader->UseBranch("MissingET");

    int i = 0;
    int nFilled = 0;
    int nAvailableEvents = reader->GetEntries();
    while (nFilled < nEvents) {
        if (i > nAvailableEvents) {
            std::cerr << "Files didn't contain enough entries.\n";
            break;
        }

        reader->ReadEntry(i);
        i++;
        TLorentzVector *positiveJet = new TLorentzVector();
        TLorentzVector *negativeJet = new TLorentzVector();
        bool found_tag_jets = FindTagJetPair(classifier, jetBranch, positiveJet, negativeJet);

        TLorentzVector *lepton = new TLorentzVector();
        bool found_lepton = FindLepton(electronBranch, muonBranch, lepton);

        TLorentzVector *hadronicJet = FindHadronicJet(jetBranch);
        if (!(found_tag_jets && found_lepton && hadronicJet)) {
            cerr << "Event didn't pass.\n";
            continue;
        }
        nFilled++;
        TLorentzVector *tagJetPair = new TLorentzVector(*positiveJet + *negativeJet);
        WWScatteringEvent *event = (WWScatteringEvent*)eventBranch->NewEntry();
        event->HadronicJetAbsEta = abs(hadronicJet->Eta());
        event->HadronicJetMass = hadronicJet->M();
        event->HadronicJetPT = hadronicJet->Pt();
        event->MissingET = missingET->MET;
        event->Mjj = tagJetPair->M();
        event->LeptonAbsEta = abs(lepton->Eta());
        event->LeptonPT = lepton->Pt();
        writer->Fill();
        writer->Clear();
    }
    writer->Write();
}

int main(int argc, char **argv) {
    char *ttbarFileName = NULL;
    char *wpjetsFileName = NULL;
    char *smwwFileNames[50];
    char *jetWeightsFileName = NULL;
    char *outputFileName = NULL;
    int nSMFiles = 0;
    int nTTBarEvents = 0;
    int nSMEvents = 0;
    int nWPJetsEvents = 0;
    int c;
    while(1) {
        int option_index = 0;
        static struct option long_options[] = {
            {"ttbarFile", required_argument, 0, 'a'},
            {"wpjetsFile", required_argument, 0, 'b'},
            {"smwwFile", required_argument, 0, 'c'},
            {"nSMEvents", required_argument, 0, 'd'},
            {"nTTBarEvents", required_argument, 0, 'e'},
            {"nWPJetsEvents", required_argument, 0, 'f'},
            {"jetWeights", required_argument, 0, 'g'},
            {"outputFile", required_argument, 0, 'h'}
        };
        c = getopt_long(argc, argv, "abcdefgh:", long_options, &option_index);
        if (c==-1)
            break;
        switch(c) {
            case 'a':
                ttbarFileName = optarg;
                break;
            case 'b':
                wpjetsFileName = optarg; 
                break;
            case 'c':
                smwwFileNames[nSMFiles] = optarg;
                nSMFiles++;
                break;
            case 'd':
                nSMEvents = atoi(optarg);
                break;
            case 'e':
                nTTBarEvents = atoi(optarg);
                break;
            case 'f':
                nWPJetsEvents = atoi(optarg);
                break;
            case 'g':
                jetWeightsFileName = optarg;
                break;
            case 'h':
                outputFileName = optarg;
                break;
        }
    }
    JetClassifier *jetClassifier = new JetClassifier(jetWeightsFileName);
    cerr << "Initialized jet classifier.\n";
    TFile *ttbarFile = new TFile(ttbarFileName);
    TFile *wpjetsFile = new TFile(wpjetsFileName);
    TChain *smwwChain = new TChain("Delphes");
    for (int i = 0; i < nSMFiles; i++) {
        smwwChain->Add(smwwFileNames[i]);
    }

    TFile *outputFile = new TFile(outputFileName, "RECREATE");

    TTree *ttbar = (TTree*)ttbarFile->Get("Delphes");
    TTree *wpjets = (TTree*)wpjetsFile->Get("Delphes");

    ExRootTreeReader *ttbarReader = new ExRootTreeReader(ttbar);
    ExRootTreeReader *wpjetsReader = new ExRootTreeReader(wpjets);
    ExRootTreeReader *smwwReader = new ExRootTreeReader(smwwChain);

    ExRootTreeWriter *ttbarWriter = new ExRootTreeWriter(outputFile, "TTBar");
    ExRootTreeWriter *wpjetsWriter = new ExRootTreeWriter(outputFile, "WPJets");
    ExRootTreeWriter *smwwWriter = new ExRootTreeWriter(outputFile, "SMWW");

    makeTrainingEvents(jetClassifier, ttbarReader, ttbarWriter, nTTBarEvents);
    makeTrainingEvents(jetClassifier, wpjetsReader, wpjetsWriter, nWPJetsEvents);
    makeTrainingEvents(jetClassifier, smwwReader, smwwWriter, nSMEvents);
    outputFile->Close();
}
