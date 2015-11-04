#include <stdio.h>
#include <getopt.h>
#include <iostream>
#include <assert.h>
#include <math.h>

#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"

#include "FileFormats.h"
#include "Common.h"


using namespace std;

void setupJetClassification(VBFNLOTree *truth, DelphesTree *delphes, JetClassifierTree **pileup, 
        JetClassifierTree **tagging) {

    assert(truth->tree->GetEntries() == delphes->tree->GetEntries());

    for (int i = 0; i < truth->tree->GetEntries(); i++) {
        truth->tree->GetEntry(i);
        delphes->tree->GetEntry(i);
        int nJets = delphes->jet_eta->GetLen();
        assert(delphes->jet_eta->GetLen() == delphes->jet_phi->GetLen());

        for (int j = 0; j < nJets; j++) {

            bool is_tagging = false;
            //Try to find the proton that corresponds to this tagging
            //jet in the Parton level file.
            int nParticles = truth->particle_pid->GetLen();
            for (int k = 0; k < nParticles; k++) {
                if (truth->particle_pid->GetValue(k) != PROTON)
                    continue;
                double separation = etaPhi(delphes->jet_eta->GetValue(k), delphes->jet_phi->GetValue(k), 
                        truth->particle_eta->GetValue(j), truth->particle_phi->GetValue(j));
                if (separation < MAX_ETA_PHI) {
                    //This proton corresponds to the jet.
                    is_tagging = true;
                    break;
                }
            }
            JetClassifierTree *treeToFill = is_tagging ? *tagging : *pileup;
            treeToFill->abs_eta = abs(delphes->jet_eta->GetValue(j));
            treeToFill->phi = delphes->jet_phi->GetValue(j);
            treeToFill->pt = delphes->jet_pt->GetValue(j);
            treeToFill->beta_star = delphes->jet_beta_star->GetValue(j);
            treeToFill->mass = delphes->jet_mass->GetValue(j);
            treeToFill->energy = getEnergy1(delphes->jet_pt->GetValue(j), delphes->jet_eta->GetValue(j),
                    delphes->jet_phi->GetValue(j), delphes->jet_mass->GetValue(j));
            treeToFill->tree->Fill();
        }
    }


}

int main(int argc, char **argv) {
    char *vbfnloFileName = NULL;
    char *delphesFileName = NULL;
    int c;
    while(1) {
        int option_index = 0;
        static struct option long_options[] = {
            {"--inputDelphesFile", required_argument, 0, 'a'},
            {"--inputVBFNLOFile", required_argument, 0, 'b'}
        };
        c = getopt_long(argc, argv, "a:", long_options, &option_index);
        if (c==-1)
            break;
        switch(c) {
            case 'a':
                delphesFileName = optarg;
                break;
            case 'b':
                vbfnloFileName = optarg; 
                break;
        }
    }


    TFile *delphesFile = new TFile(delphesFileName);
    TFile *vbfnloFile = new TFile(vbfnloFileName);
    DelphesTree *delphes = new DelphesTree(delphesFile);
    VBFNLOTree *vbfnlo = new VBFNLOTree(vbfnloFile);
    JetClassifierTree *pileup;
    JetClassifierTree *tagging;
    setupJetClassification(vbfnlo, delphes, &pileup, &tagging);

}
