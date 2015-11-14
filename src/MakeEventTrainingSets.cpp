#include <getopt.h>
#include <stdio.h>
#include <iostream>


#include "TMVA/Factory.h"
#include "ExRootTreeReader.h"
#include "ExRootTreeWriter.h"
#include "ExRootTreeBranch.h"

#include "Common.h"
#include "WWScatteringEvent.h"
#include "Classifiers.h"

void makeTrainingEvents(JetClassifier *classifier, ExRootTreeReader *reader, ExRootTreeWriter *writer) {
    ExRootTreeBranch *eventBranch = writer->NewBranch("WWScatteringEvent", WWScatteringEvent::Class());


    TClonesArray *jetBranch = (TClonesArray*)reader->UseBranch("Jet");
    TClonesArray *electronBranch = (TClonesArray*)reader->UseBranch("Electron");
    MissingET *missingET = (MissingET*)reader->UseBranch("MissingET");

    int nEntries = reader->GetEntries();
    for (int i = 0; i < nEntries; i++) {
        reader->ReadEntry(i);
        Jet *tagJet1;
        Jet *tagJet2;
        FindTagJetPair(classifier, jetBranch, &tagJet1, &tagJet2);
        Electron *electron = FindElectron(electronBranch);
        Jet *hadronicJet = FindHadronicJet(jetBranch);
        if (!(tagJet1 && tagJet2 && electron && hadronicJet)) {
            //Couldn't find all the required components for a WW-scattering
            //event.
            continue;
        }
        WWScatteringEvent *event = (WWScatteringEvent*)eventBranch->NewEntry();
        event->HadronicJetAbsEta = abs(hadronicJet->Eta);
        event->HadronicJetMass = hadronicJet->Mass;
        event->HadronicJetPT = hadronicJet->PT;
        event->MissingET = missingET->MET;
        event->Mjj = JetPairInvariantMass(tagJet1, tagJet2);
        event->LeptonAbsEta = abs(electron->Eta);
        event->LeptonPT = electron->PT;
        writer->Fill();
    }
}

int main(int argc, char **argv) {
    char *ttbarFileName = NULL;
    char *wpjetsFileName = NULL;
    char *smwwFileName = NULL;
    char *weightsFileName = NULL;
    char *outputFileName = NULL;
    int c;
    while(1) {
        int option_index = 0;
        static struct option long_options[] = {
            {"ttbarFile", required_argument, 0, 'a'},
            {"wpjetsFile", required_argument, 0, 'b'},
            {"smwwFile", required_argument, 0, 'c'},
            {"weightsFile", required_argument, 0, 'd'},
            {"outputFile", required_argument, 0, 'e'}
        };
        c = getopt_long(argc, argv, "abcde:", long_options, &option_index);
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
                smwwFileName = optarg;
                break;
            case 'd':
                weightsFileName = optarg;
                break;
            case 'e':
                outputFileName = optarg;
                break;
        }
    }
    JetClassifier *jetClassifier = new JetClassifier(weightsFileName);
    TFile *ttbarFile = new TFile(ttbarFileName);
    TFile *wpjetsFile = new TFile(wpjetsFileName);
    TFile *smwwFile = new TFile(smwwFileName);

    TFile *outputFile = new TFile(outputFileName, "RECREATE");

    TTree *ttbar = (TTree*)ttbarFile->Get("Delphes");
    TTree *wpjets = (TTree*)wpjetsFile->Get("Delphes");
    TTree *smww = (TTree*)smwwFile->Get("Delphes");

    ExRootTreeReader *ttbarReader = new ExRootTreeReader(ttbar);
    ExRootTreeReader *wpjetsReader = new ExRootTreeReader(wpjets);
    ExRootTreeReader *smwwReader = new ExRootTreeReader(smww);

    ExRootTreeWriter *ttbarWriter = new ExRootTreeWriter(outputFile, "TTBar");
    ExRootTreeWriter *wpjetsWriter = new ExRootTreeWriter(outputFile, "WPJets");
    ExRootTreeWriter *smwwWriter = new ExRootTreeWriter(outputFile, "SMWW");

    makeTrainingEvents(jetClassifier, ttbarReader, ttbarWriter);
    makeTrainingEvents(jetClassifier, wpjetsReader, wpjetsWriter);
    makeTrainingEvents(jetClassifier, smwwReader, smwwWriter);
    outputFile->Close();
}
