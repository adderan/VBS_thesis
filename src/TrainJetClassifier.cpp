#include <getopt.h>
#include <iostream>

#include "TMVA/Types.h"
#include "TFile.h"
#include "TClonesArray.h"
#include "TMVA/Factory.h"


int main(int argc, char **argv) {
    char *jetTrainingFileName = NULL;
    char *outputFileName = NULL;
    int c;
    while(1) {
        int option_index = 0;
        static struct option long_options[] = {
            {"inputJetTrainingFile", required_argument, 0, 'a'},
            {"outputFile", required_argument, 0, 'b'}
        };
        c = getopt_long(argc, argv, "ab:", long_options, &option_index);
        if (c==-1)
            break;
        switch(c) {
            case 'a':
                jetTrainingFileName = optarg;
                break;
            case 'b':
                outputFileName = optarg;
                break;
        }
    }

    std::cerr << outputFileName;
    TFile *trainingFile = new TFile(jetTrainingFileName);
    TTree *tagJets = (TTree*)trainingFile->Get("Tagging");
    TTree *backgroundJets = (TTree*)trainingFile->Get("Background");
    TFile *outputFile = new TFile(outputFileName, "RECREATE");


    TMVA::Factory *factory = new TMVA::Factory("JetMatching", outputFile);
    factory->AddSignalTree(tagJets, 1.0);
    factory->AddBackgroundTree(backgroundJets, 1.0);

    factory->AddVariable("TrainJet.PT");
    factory->AddVariable("TrainJet.AbsEta");
    factory->AddVariable("TrainJet.Mass");
    factory->AddVariable("TrainJet.BetaStar");
    factory->AddVariable("TrainJet.Energy");

    factory->PrepareTrainingAndTestTree("", "", "nTrain_Signal=0:nTrain_Background=0:SplitMode=Random:NormMode=NumEvents:!V");
    factory->BookMethod( TMVA::Types::kBDT, "BDTG",
        "!H:!V:NTrees=1000:BoostType=Grad:Shrinkage=0.10:UseBaggedGrad:GradBaggingFraction=0.5:nCuts=20:NNodesMax=5" );

    factory->TrainAllMethods();
    outputFile->Close();

    delete factory;

}
