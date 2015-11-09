#include <stdio.h>
#include <getopt.h>
#include <iostream>
#include <assert.h>
#include <math.h>

#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TClonesArray.h"

#include "Common.h"

#include "ExRootTreeReader.h"
#include "classes/DelphesClasses.h"
#include "ExRootClasses.h"



using namespace std;

/*
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
                if (truth->particle_pid->GetValue(k) > MAX_QUARK) {
                    continue;
                }
                if (truth->particle_status->GetValue(k) != 1) {
                    continue;
                }
                double separation = etaPhi(delphes->jet_eta->GetValue(k), delphes->jet_phi->GetValue(k), 
                        truth->particle_eta->GetValue(j), truth->particle_phi->GetValue(j));
                if (separation < MAX_ETA_PHI) {
                    //This proton corresponds to the jet.
                    cerr << "Found corresponding proton " << k << " for jet " << j << "\n";
                    is_tagging = true;
                    break;
                }
            }
            if (!is_tagging) {
                cerr << "Not a tagging jet\n";
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
*/
int main(int argc, char **argv) {
    char *vbfnloFileName = NULL;
    char *delphesFileName = NULL;
    int c;
    while(1) {
        int option_index = 0;
        static struct option long_options[] = {
            {"inputDelphesFile", required_argument, 0, 'a'},
            {"inputVBFNLOFile", required_argument, 0, 'b'}
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
    TTree *vbfnlo = (TTree*)vbfnloFile->Get("LHEF");
    TTree *delphes = (TTree*)delphesFile->Get("Delphes");
    ExRootTreeReader *vbfnloReader = new ExRootTreeReader(vbfnlo);
    ExRootTreeReader *delphesReader = new ExRootTreeReader(delphes);
    TClonesArray *jetBranch = delphesReader->UseBranch("Jet");
    TClonesArray *particleBranch = vbfnloReader->UseBranch("Particle");
    delphesReader->ReadEntry(0);
    vbfnloReader->ReadEntry(0);
    Jet *firstJet = (Jet*)jetBranch->At(0);
    TRootLHEFParticle *firstParticle = (TRootLHEFParticle*)particleBranch->At(0);
    cerr << "Jet pt: " << firstJet->PT << "\n";
    cerr << "Particle pt: " << firstParticle->PT << "\n";
    cerr << "Particle pid: " << firstParticle->PID << "\n";

    TLeaf *particle_pt = vbfnlo->GetLeaf("Particle.PT");
    TLeaf *particle_pid = vbfnlo->GetLeaf("Particle.PID");
    vbfnlo->GetEntry(0);
    cerr << "Actual pt: " << particle_pt->GetValue(0) << "\n";
    cerr << "Actual pid: " << particle_pid->GetValue(0) << "\n";

}
