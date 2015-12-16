#include <iostream>
#include <getopt.h>

#include "TMVA/Factory.h"
#include "TFile.h"


int main(int argc, char **argv) {
    char *eventTrainingFileName = NULL;
    char *outputFileName = NULL;
    int c;
    while(1) {
        int option_index = 0;
        static struct option long_options[] = {
            {"eventTrainFile", required_argument, 0, 'a'},
            {"outputFile", required_argument, 0, 'b'}
        };
        c = getopt_long(argc, argv, "ab:", long_options, &option_index);
        if (c==-1)
            break;
        switch(c) {
            case 'a':
                eventTrainingFileName = optarg;
                break;
            case 'b':
                outputFileName = optarg;
                break;
        }
    }
    TFile *file = new TFile(eventTrainingFileName);
    TTree *ttbar = (TTree*)file->Get("TTBar");
    TTree *wp3jets = (TTree*)file->Get("WP3Jets");
    TTree *wp4jets = (TTree*)file->Get("WP4Jets");
    TTree *sm = (TTree*)file->Get("SMWW");

    TFile *outputFile = new TFile(outputFileName, "RECREATE");
    TMVA::Factory *factory = new TMVA::Factory("EventClassification", outputFile);
    factory->AddSignalTree(sm);
    factory->AddBackgroundTree(ttbar);
    factory->AddBackgroundTree(wp3jets);
    factory->AddBackgroundTree(wp4jets);

    factory->AddVariable("WWScatteringEvent.HadronicJetAbsEta");
    factory->AddVariable("WWScatteringEvent.HadronicJetPT");
    factory->AddVariable("WWScatteringEvent.HadronicJetMass");
    factory->AddVariable("WWScatteringEvent.MissingET");
    factory->AddVariable("WWScatteringEvent.Mjj");
    factory->AddVariable("WWScatteringEvent.LeptonAbsEta");
    factory->AddVariable("WWScatteringEvent.LeptonPT");

    factory->PrepareTrainingAndTestTree("", "", "nTrain_Signal=0:nTrain_Background=0:SplitMode=Random:NormMode=NumEvents:!V");
    factory->BookMethod( TMVA::Types::kBDT, "BDT",
						   "!H:!V:NTrees=850:MinNodeSize=2.5%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=20" );
    factory->TrainAllMethods();
    
    factory->TestAllMethods();
    factory->EvaluateAllMethods();
    outputFile->Close();
}

