#include <stdlib.h>

#include "Common.h"
#include "TLorentzVector.h"
#include "TClonesArray.h"

#include "ExRootClasses.h"
#include "classes/DelphesClasses.h"

#include "Classifiers.h"


//Find the separation between two points in Eta-phi space
double deltaR(double eta1, double phi1, double eta2, double phi2) {
    return sqrt((eta1 - eta2)*(eta1 - eta2) + (phi1 - phi2)*(phi1 - phi2));
}


double GetJetEnergy(Jet *jet) {
    TLorentzVector *tlv = new TLorentzVector();
    tlv->SetPtEtaPhiM(jet->PT, jet->Eta, jet->Phi, jet->Mass);
    return tlv->E();
}

//Finds the two tagging high-eta tagging jets on opposite eta hemispheres, 
//using the provided tagging jet classifier
void FindTagJetPair(JetClassifier *classifier, TClonesArray *jets, Jet **tagJet1, Jet **tagJet2) {
    int nJets = jets->GetEntriesFast();
    for(int i = 0; i < nJets; i++) {
        Jet *jet = (Jet*)jets->At(i);
        if (classifier->isTaggingJet(jet)) {
            if (!*tagJet1) {
                *tagJet1 = jet;
            }
            else if (!*tagJet2) {
                *tagJet2 = jet;
            }
            else {
                std::cerr << "Warning: Too many tag jets in this event.\n";
                *tagJet1 = NULL;
                *tagJet2 = NULL;
                return;
            }
        }
    }
}

//Find the highest PT electron in an event
Electron *FindElectron(TClonesArray *electrons) {
    int nElectrons = electrons->GetEntriesFast();
    Electron *maxPTElectron = NULL;
    Electron *minEtaElectron = NULL;

    double max_pt = 0;
    double min_eta = 10000;
    for (int i = 0; i < nElectrons; i++) {
        Electron *electron = (Electron*)electrons->At(i);
        if (electron->PT > max_pt) {
            maxPTElectron = electron;
            max_pt = electron->PT;
        }
        if (abs(electron->Eta) < min_eta) {
            minEtaElectron = electron;
            min_eta = abs(electron->Eta);
        }
    }
    if (maxPTElectron != minEtaElectron) {
        std::cerr << "Warning: Min abs(eta) and max PT disagree.\n";
        return NULL;
    }
    return maxPTElectron;
}

//Find the hadronic jet in an event, where
//the hadronic jet is defined as having the highest PT
//and lowest abs(Eta)
Jet *FindHadronicJet(TClonesArray *jets) {
    Float_t min_eta = 100000;
    Float_t max_pt = 0;
    Jet *minEtaJet = NULL;
    Jet *maxPTJet = NULL;

    int nJets = jets->GetEntriesFast();
    for (int i = 0; i < nJets; i++) {
        Jet *jet = (Jet*)jets->At(i);
        if (abs(jet->Eta) < min_eta) {
            minEtaJet = jet;
        }
        if (jet->PT > max_pt) {
            maxPTJet = jet;
        }
    }
    if (minEtaJet != maxPTJet) {
        std::cerr << "Warning: Min abs(Eta) jet and max PT jet are different.\n";
        return NULL;
    }
    return minEtaJet;
}

Float_t JetPairInvariantMass(Jet *jet1, Jet *jet2) {
    TLorentzVector *tlv1 = new TLorentzVector();
    tlv1->SetPtEtaPhiM(jet1->PT, jet1->Eta, jet1->Phi, jet1->Mass);
    TLorentzVector *tlv2 = new TLorentzVector();
    tlv2->SetPtEtaPhiM(jet2->PT, jet2->Eta, jet2->Phi, jet2->Mass);
    TLorentzVector tlv3 = *tlv1 + *tlv2;
    return tlv3.M();
}
