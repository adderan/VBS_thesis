#include <stdio.h>
#include <iostream>
#include <getopt.h>

#include "TFile.h"
#include "TTree.h"
#include "ExRootTreeReader.h"
#include "classes/DelphesClasses.h"
#include "TClonesArray.h"

#include "Classifiers.h"

using namespace std;

int main(int argc, char **argv) {
    char *weightsFileName = NULL;
    char *eventsFileName = NULL;
    int c;
    while(1) {
        int option_index = 0;
        static struct option long_options[] = {
            {"inputWeightsFile", required_argument, 0, 'a'},
            {"inputEventsFile", required_argument, 0, 'b'}
        };
        c = getopt_long(argc, argv, "ab:", long_options, &option_index);
        if (c==-1)
            break;
        switch(c) {
            case 'a':
                weightsFileName = optarg;
                break;
            case 'b':
                eventsFileName = optarg; 
                break;
        }
    }
    JetClassifier *classifier = new JetClassifier(weightsFileName);
    cerr << "Successfully created classifier\n";
    TFile *file = new TFile(eventsFileName);
    TTree *tree = (TTree*)file->Get("Delphes");
    ExRootTreeReader *reader = new ExRootTreeReader(tree);
    TClonesArray *jetBranch = (TClonesArray*)reader->UseBranch("Jet");

    reader->ReadEntry(0);
    int nJets = jetBranch->GetEntriesFast();
    for (int i = 0; i < nJets; i++) {
        cerr << "Processing entry " << i << "\n";
        Jet *jet = (Jet*)jetBranch->At(i);
        cerr << "Jet is tagging jet: " << classifier->isTaggingJet(jet);
    }
}
    

