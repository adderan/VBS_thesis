#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>


#include "TMVA/Factory.h"
#include "ExRootTreeReader.h"
#include "ExRootTreeWriter.h"
#include "ExRootTreeBranch.h"
#include "TClonesArray.h"

#include "Common.h"
#include "WWScatteringEvent.h"
#include "Classifiers.h"

using namespace std;

void makeTrainingEvents(JetClassifier *classifier, ExRootTreeReader *reader, 
        ExRootTreeWriter *writer, int start, int stop) {
    ExRootTreeBranch *eventBranch = writer->NewBranch("WWScatteringEvent", 
            WWScatteringEvent::Class());

    TClonesArray *jetBranch = (TClonesArray*)reader->UseBranch("Jet");
    TClonesArray *muonBranch = (TClonesArray*)reader->UseBranch("Muon");
    TClonesArray *electronBranch = (TClonesArray*)reader->UseBranch("Electron");
    TClonesArray *missingETBranch = (TClonesArray*)reader->UseBranch("MissingET");

    int nFilled = 0;
    for (int i = start; i < stop; i++) {
        reader->ReadEntry(i);
        struct WWScatteringComponents *components = new WWScatteringComponents(classifier, electronBranch,
                muonBranch, jetBranch, missingETBranch);
        if(!components->isGoodEvent) {
            std::cerr << "Event didn't have the required components.\n";
            continue;
        }
        nFilled++;

        TLorentzVector *tagJetPair = new TLorentzVector(*components->positiveJet + *components->negativeJet);
        WWScatteringEvent *event = (WWScatteringEvent*)eventBranch->NewEntry();
        event->HadronicJetAbsEta = abs(components->hadronicJet->Eta());
        event->HadronicJetMass = components->hadronicJet->M();
        event->HadronicJetPT = components->hadronicJet->Pt();
        event->MissingET = components->missingET->E();
        event->Mjj = tagJetPair->M();
        event->LeptonAbsEta = abs(components->lepton->Eta());
        event->LeptonPT = components->lepton->Pt();
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
    int start, stop;
    int c;
    while(1) {
        int option_index = 0;
        static struct option long_options[] = {
            {"ttbarFile", required_argument, 0, 'a'},
            {"wpjetsFile", required_argument, 0, 'b'},
            {"smwwFile", required_argument, 0, 'c'},
            {"jetWeights", required_argument, 0, 'd'},
            {"outputFile", required_argument, 0, 'e'},
            {"start", required_argument, 0, 'f'},
            {"stop", required_argument, 0, 'g'}
        };
        c = getopt_long(argc, argv, "abcdefg:", long_options, &option_index);
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
                jetWeightsFileName = optarg;
                break;
            case 'e':
                outputFileName = optarg;
                break;
            case 'f':
                start = atoi(optarg);
                break;
            case 'g':
                stop = atoi(optarg);
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

    makeTrainingEvents(jetClassifier, ttbarReader, ttbarWriter, start, stop);
    makeTrainingEvents(jetClassifier, wpjetsReader, wpjetsWriter, start, stop);
    makeTrainingEvents(jetClassifier, smwwReader, smwwWriter, start, stop);
    outputFile->Close();
}
