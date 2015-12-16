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
    char *wp3jetsFileName = NULL;
    char *wp4jetsFileName = NULL;
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
            {"wp3jetsFile", required_argument, 0, 'b'},
            {"wp4jetsFile", required_argument, 0, 'c'},
            {"smwwFile", required_argument, 0, 'd'},
            {"jetWeights", required_argument, 0, 'e'},
            {"outputFile", required_argument, 0, 'f'},
            {"start", required_argument, 0, 'g'},
            {"stop", required_argument, 0, 'h'}
        };
        c = getopt_long(argc, argv, "abcdefgh:", long_options, &option_index);
        if (c==-1)
            break;
        switch(c) {
            case 'a':
                ttbarFileName = optarg;
                break;
            case 'b':
                wp3jetsFileName = optarg; 
                break;
            case 'c':
                wp4jetsFileName = optarg;
                break;
            case 'd':
                smwwFileNames[nSMFiles] = optarg;
                nSMFiles++;
                break;
            case 'e':
                jetWeightsFileName = optarg;
                break;
            case 'f':
                outputFileName = optarg;
                break;
            case 'g':
                start = atoi(optarg);
                break;
            case 'h':
                stop = atoi(optarg);
                break;
        }
    }
    JetClassifier *jetClassifier = new JetClassifier(jetWeightsFileName);
    cerr << "Initialized jet classifier.\n";
    TFile *ttbarFile = new TFile(ttbarFileName);
    TFile *wp3jetsFile = new TFile(wp3jetsFileName);
    TFile *wp4jetsFile = new TFile(wp4jetsFileName);
    TChain *smwwChain = new TChain("Delphes");
    for (int i = 0; i < nSMFiles; i++) {
        smwwChain->Add(smwwFileNames[i]);
    }

    TFile *outputFile = new TFile(outputFileName, "RECREATE");

    TTree *ttbar = (TTree*)ttbarFile->Get("Delphes");
    TTree *wp3jets = (TTree*)wp3jetsFile->Get("Delphes");
    TTree *wp4jets = (TTree*)wp4jetsFile->Get("Delphes");

    ExRootTreeReader *ttbarReader = new ExRootTreeReader(ttbar);
    ExRootTreeReader *wp3jetsReader = new ExRootTreeReader(wp3jets);
    ExRootTreeReader *wp4jetsReader = new ExRootTreeReader(wp4jets);
    ExRootTreeReader *smwwReader = new ExRootTreeReader(smwwChain);

    ExRootTreeWriter *ttbarWriter = new ExRootTreeWriter(outputFile, "TTBar");
    ExRootTreeWriter *wp3jetsWriter = new ExRootTreeWriter(outputFile, "WP3Jets");
    ExRootTreeWriter *wp4jetsWriter = new ExRootTreeWriter(outputFile, "WP4Jets");
    ExRootTreeWriter *smwwWriter = new ExRootTreeWriter(outputFile, "SMWW");

    makeTrainingEvents(jetClassifier, ttbarReader, ttbarWriter, start, stop);
    makeTrainingEvents(jetClassifier, wp3jetsReader, wp3jetsWriter, start, stop);
    makeTrainingEvents(jetClassifier, wp4jetsReader, wp4jetsWriter, start, stop);
    makeTrainingEvents(jetClassifier, smwwReader, smwwWriter, start, stop);
    outputFile->Close();
}
