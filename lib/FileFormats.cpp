#include "FileFormats.h"
#include "TTree.h"
#include "TLeaf.h"
#include "TFile.h"


/* Abstract Tree */


/* Delphes Tree */

DelphesTree::DelphesTree(TFile *f) {
    tree = (TTree*)f->Get("Delphes");
    jet_eta = tree->GetLeaf("Jet.Eta");
    jet_phi = tree->GetLeaf("Jet.Phi");
    jet_pt = tree->GetLeaf("Jet.PT");
    jet_mass = tree->GetLeaf("Jet.Mass");
    jet_beta_star = tree->GetLeaf("Jet.BetaStar");
}

/*VBFNLOTree */
VBFNLOTree::VBFNLOTree(TFile *f) {
    tree = (TTree*)f->Get("LHEF");
    particle_eta = tree->GetLeaf("Particle.Eta");
    particle_phi = tree->GetLeaf("Particle.Phi");
    particle_pt = tree->GetLeaf("Partcile.PT");
    particle_pid = tree->GetLeaf("Particle.PID");
}


/* JetClassifierTee */
JetClassifierTree::JetClassifierTree() {
    tree = new TTree();
    tree->Branch("AbsEta", &abs_eta);
    tree->Branch("Phi", &phi);
    tree->Branch("PT", &pt);
    tree->Branch("BetaStar", &beta_star);
    tree->Branch("Mass", &mass);
    tree->Branch("Energy", &energy);
}

