#include <stdio.h>
#include <getopt.h>
#include <iostream>
#include <assert.h>
#include <math.h>

#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"

#include "Common.h"

#include "ExRootTreeReader.h"
#include "ExRootTreeWriter.h"
#include "classes/DelphesClasses.h"
#include "ExRootClasses.h"
#include "ExRootTreeBranch.h"
#include "TrainJet.h"


using namespace std;

void FillTrainJet(Jet *jet, TrainJet *trainJet) {
    trainJet->BetaStar = jet->BetaStar;
    trainJet->AbsEta = abs(jet->Eta);
    trainJet->Mass = jet->Mass;
    trainJet->PT = jet->PT;
    TLorentzVector *v = new TLorentzVector();
    v->SetPtEtaPhiM(jet->PT, jet->Eta, jet->Phi, jet->Mass);
    trainJet->Energy = v->E();
}

void createTrainingSets(ExRootTreeReader *vbfnloReader, ExRootTreeReader *delphesReader, 
        ExRootTreeWriter *taggingJets, ExRootTreeWriter *backgroundJets) {
    //assert(vbfnloReader->GetEntries() == delphesReader->GetEntries());

    ExRootTreeBranch *taggingJetsBranch = taggingJets->NewBranch("TrainJet", TrainJet::Class());
    ExRootTreeBranch *backgroundJetsBranch = backgroundJets->NewBranch("TrainJet", TrainJet::Class());


    TClonesArray *jetBranch = delphesReader->UseBranch("Jet");
    TClonesArray *particleBranch = vbfnloReader->UseBranch("Particle");
    
    for (int i = 0; i < delphesReader->GetEntries(); i++) {
        cerr << "Processing event: " << i << "\n";
        vbfnloReader->ReadEntry(i);
        delphesReader->ReadEntry(i);
        
        //Find the tagging partons for this event
        TRootLHEFParticle *positiveParton = NULL;
        TRootLHEFParticle *negativeParton = NULL;
        Float_t positive_maxEta = 0.0;
        Float_t negative_minEta = 0.0;
        int nParticles = particleBranch->GetEntriesFast();
        for (int k = 0; k < nParticles; k++) {
            TRootLHEFParticle *particle = (TRootLHEFParticle*)particleBranch->At(k);
            if (particle->PID > MAX_QUARK) {
                continue;
            }
            if (particle->Status != 1) {
                continue;
            }
            if (particle->Eta > positive_maxEta) {
                positiveParton = particle;
                positive_maxEta = particle->Eta;
            }
            else if (particle->Eta < negative_minEta) {
                negativeParton = particle;
                negative_minEta = particle->Eta;
            }

        }
        //Try to match the tagging partons with tagging jets in the Delphes output
        int nJets = jetBranch->GetEntriesFast();
        for (int i = 0; i < nJets; i++) {
            Jet *jet = (Jet*)jetBranch->At(i);
            if((positiveParton && deltaR(jet->Eta, jet->Phi, positiveParton->Eta, positiveParton->Phi) < MAX_DELTA_R) ||
                    (negativeParton && deltaR(jet->Eta, jet->Phi, negativeParton->Eta, 
                                              negativeParton->Phi) < MAX_DELTA_R)) {
                cerr << "Matched tagging parton\n";
                TrainJet *newJet = (TrainJet*)taggingJetsBranch->NewEntry();
                FillTrainJet(jet, newJet);
                taggingJets->Fill();
                taggingJets->Clear();
            }
            else {
                cerr << "Jet didn't match tagging parton\n";
                TrainJet *newJet = (TrainJet*)backgroundJetsBranch->NewEntry();
                FillTrainJet(jet, newJet);
                backgroundJets->Fill();
                backgroundJets->Clear();
            }
        }


    }


}

int main(int argc, char **argv) {
    char *vbfnloFileName = NULL;
    char *delphesFileName = NULL;
    char *outputFileName = NULL;
    int c;
    while(1) {
        int option_index = 0;
        static struct option long_options[] = {
            {"inputDelphesFile", required_argument, 0, 'a'},
            {"inputVBFNLOFile", required_argument, 0, 'b'},
            {"outputFile", required_argument, 0, 'c'}
        };
        c = getopt_long(argc, argv, "abc:", long_options, &option_index);
        if (c==-1)
            break;
        switch(c) {
            case 'a':
                delphesFileName = optarg;
                break;
            case 'b':
                vbfnloFileName = optarg; 
                break;
            case 'c':
                outputFileName = optarg;
                break;
        }
    }

    TFile outputFile(outputFileName, "RECREATE");
    TFile *delphesFile = new TFile(delphesFileName);
    TFile *vbfnloFile = new TFile(vbfnloFileName);
    TTree *vbfnlo = (TTree*)vbfnloFile->Get("LHEF");
    TTree *delphes = (TTree*)delphesFile->Get("Delphes");
    ExRootTreeReader *vbfnloReader = new ExRootTreeReader(vbfnlo);
    ExRootTreeReader *delphesReader = new ExRootTreeReader(delphes);
    ExRootTreeWriter *tagJets = new ExRootTreeWriter(&outputFile, "Tagging");
    ExRootTreeWriter *backgroundJets = new ExRootTreeWriter(&outputFile, "Background");

    createTrainingSets(vbfnloReader, delphesReader, tagJets, backgroundJets);
    tagJets->Write();
    //backgroundJets->Write();
    outputFile.Close();
}
