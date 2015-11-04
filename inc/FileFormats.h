#ifndef FILE_FORMATS_H
#define FILE_FORMATS_H

#include "TTree.h"

class AbstractTree {
    public:
        TTree *tree;
};

class DelphesTree: public AbstractTree {
    public:
        DelphesTree(TFile *f);
        TLeaf *jet_eta, *jet_phi, *jet_pt, *jet_beta_star, *jet_mass;
};

class VBFNLOTree: public AbstractTree {
    public:
        VBFNLOTree(TFile *f);
        TLeaf *particle_eta, *particle_phi, *particle_pid, *particle_pt;
};

class JetClassifierTree: public AbstractTree {
    public:
        double abs_eta, phi, pt, beta_star, mass, energy;
        JetClassifierTree();
};

#endif
