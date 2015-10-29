#include "TFile.h"
#include "TTree.h"
#include <stdio.h>
#include <getopt.h>
#include <iostream>

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

