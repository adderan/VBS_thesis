#include <iostream>
#include <getopt.h>

#include "ExRootTreeReader.h"
#include "ExRootClasses.h"

#include "TTree.h"
#include "TFile.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TH1F.h"

#include "Common.h"

int main(int argc, char **argv) {
    char *eventsFileName;
    int c;
    while(1) {
        int option_index = 0;
        static struct option long_options[] = {
            {"events", required_argument, 0, 'a'},
        };
        c = getopt_long(argc, argv, "a:", long_options, &option_index);
        if (c==-1)
            break;
        switch(c) {
            case 'a':
                eventsFileName = optarg;
                break;
        }
    }

    TFile *eventsFile = new TFile(eventsFileName);
    TTree *tree = (TTree*)eventsFile->Get("LHEF");
    ExRootTreeReader *reader = new ExRootTreeReader(tree);
    TClonesArray *particles = (TClonesArray*)reader->UseBranch("Particle");

    int nEntries = reader->GetEntries();
    for (int i = 0; i < nEntries; i++) {
        reader->ReadEntry(i);
        TLorentzVector *lepton = NULL;
        TLorentzVector *neutrino = NULL;
        TLorentzVector *quark1 = NULL;
        TLorentzVector *quark2 = NULL;
        TLorentzVector *w1 = NULL;
        TLorentzVector *w2 = NULL;
        double min_abs_eta = 10000;
        for (int j = 0; j < particles->GetEntriesFast(); j++) {
            TRootLHEFParticle *particle = (TRootLHEFParticle*)particles->At(j);

            if (particle->Status == -1) continue;

            TRootLHEFParticle *mother = NULL;
            if(particle->Mother1 > 0) {
                mother = (TRootLHEFParticle*)particles->At(particle->Mother1 - 1);
            }

            if (particle->PID == 24) {
                w1 = ParticleToVector(particle);
            }
            else if (particle->PID == -24) {
                w2 = ParticleToVector(particle);
            }

            else if (abs(particle->PID) < MAX_QUARK || abs(particle->PID) == GLUON) {
                if (abs(particle->Eta) < min_abs_eta) {
                    min_abs_eta = abs(particle->Eta);
                    quark1 = ParticleToVector(particle);
                }
            }
            else if (mother && abs(mother->PID) == W_BOSON) {
                if (abs(particle->PID) == ELECTRON_NEUTRINO || abs(particle->PID) == MUON_NEUTRINO) {
                    neutrino = ParticleToVector(particle);
                }
                if(abs(particle->PID) == ELECTRON || abs(particle->PT) == MUON) {
                    lepton = ParticleToVector(particle);
                }
            }

        }
        //Find second most central jet
        double second_min_abs_eta = 10000;
        for (int k = 0; k < particles->GetEntriesFast(); k++) {
            TRootLHEFParticle *particle = (TRootLHEFParticle*)particles->At(k);
            if (particle->Status == -1) continue;
            if (abs(particle->PID) < MAX_QUARK || abs(particle->PID) == GLUON) {
                if (abs(particle->Eta) < second_min_abs_eta && abs(particle->Eta) != min_abs_eta) {
                    second_min_abs_eta = abs(particle->Eta);
                    quark2 = ParticleToVector(particle);
                }
            }
        }

        if (!(lepton && neutrino && quark1 && quark2 && w1 && w2)) {
            continue;
        }

        //apprximate missing ET
        TLorentzVector *met = new TLorentzVector();
        met->SetPtEtaPhiE(neutrino->Pt(), neutrino->Eta(), neutrino->Phi(), neutrino->Pt());
        TLorentzVector *reconstructedNeutrino = ReconstructNeutrinoAlternate(met, lepton);
        TLorentzVector *reconstructedWW = ReconstructWW(lepton, quark1, met);

        TLorentzVector *trueWW = new TLorentzVector(*w1 + *w2);
        if (!reconstructedNeutrino) continue;
        std::cerr << "Truth n pz: " << neutrino->Pz() << ", Rec. n pz: " 
            << reconstructedNeutrino->Pz() << " Truth ww: " << trueWW->M() << " rec. WW: " << reconstructedWW->M() << "\n";

    }

}

