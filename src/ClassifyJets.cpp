#include "TFile.h"
#include "TTree.h"
#include <stdio.h>
#include <getopt.h>
#include <iostream>
#include <assert.h>

using namespace std;



int main(int argc, char **argv) {
    char *fileName = NULL;
    int c;
    while(1) {
        int option_index = 0;
        static struct option long_options[] = {
            {"inputFile", required_argument, 0, 'a' }
        };
        c = getopt_long(argc, argv, "a:", long_options, &option_index);
        if (c==-1)
            break;
        switch(c) {
            case 'a':
                fileName = optarg;
                break;
        }
    }


    fprintf(stderr, "Using input file %s\n", fileName);
    TFile *file = new TFile(fileName);
    TTree *tree = (TTree*)file->Get("LHEF");
    cerr << "Tree contains " << tree->GetEntries() << "\n";
}

TTree *match_jets(TTree *truth, TTree *delphes) {
    assert(truth->GetEntries() == delphes->GetEntries());
    TLeaf *delphes_eta = (TLeaf*)delphes->GetLeaf("Jet.Eta");
    TLeaf *delphes_phi = (TLeeaf*)delphes->GetLeaf("Jet.Phi");
    TLeaf *truth_eta = (TLeaf*)truth->GetLeaf("Particle.Eta");
    TLeaf *truth_phi = (TLeeaf*)truth->GetLeaf("Particle.Phi");

    for (int i = 0; i < truth->GetEntries(); i++) {
        truth->GetEntry(i);
        delphes->GetEntry(i);



}


