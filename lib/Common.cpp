#include <stdlib.h>
#include <assert.h>

#include "Common.h"
#include "TLorentzVector.h"
#include "TClonesArray.h"

#include "ExRootClasses.h"
#include "classes/DelphesClasses.h"

#include "Classifiers.h"

using namespace std;

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
            if (!(*tagJet1)) {
                *tagJet1 = jet;
            }
            else if (!(*tagJet2)) {
                *tagJet2 = jet;
            }
            else {
                cerr << "Warning: Too many tag jets in this event.\n";
                *tagJet1 = NULL;
                *tagJet2 = NULL;
                return;
            }
        }
    }
    if(!*tagJet1 && !*tagJet2) cerr << "Warning. Couldn't find the tag jets.\n";
}

//Find the highest PT electron or muon in an event
bool FindLepton(TClonesArray *electrons, TClonesArray *muons, Electron **electron, Muon **muon) {
    int nElectrons = electrons->GetEntriesFast();
    int nMuons = muons->GetEntriesFast();
    cerr << "nElectrons = " << nElectrons << "\n";
    cerr << "nMuons = " << nMuons << "\n";
    Float_t max_pt = 0.0;

    for (int i = 0; i < nElectrons; i++) {
        Electron *candidate_electron = (Electron*)electrons->At(i);
        if(abs(candidate_electron->Eta) > LEPTON_ETA_CUTOFF) continue;

        if (candidate_electron->PT > max_pt) {
            max_pt = candidate_electron->PT;
            *electron = candidate_electron;
            *muon = NULL;
        }
    }
    for (int i = 0; i < nMuons; i++) {
        Muon *candidate_muon = (Muon*)muons->At(i);
        if (abs(candidate_muon->Eta) > LEPTON_ETA_CUTOFF) continue;

        if (candidate_muon->PT > max_pt) {
            max_pt = candidate_muon->PT;
            *muon = candidate_muon;
            electron = NULL;
        }
    }
    if ((*electron) || (*muon)) return true;
    cerr << "Warning: Couldn't find the lepton.\n";
    return false;
}

//Find the highest PT anti-kt 10 jet in the
//event
Jet *FindHadronicJet(TClonesArray *jets) {
    Float_t max_pt = 0;
    Jet *maxPTJet = NULL;

    int nJets = jets->GetEntriesFast();
    for (int i = 0; i < nJets; i++) {
        Jet *jet = (Jet*)jets->At(i);
        Double_t dR = sqrt(jet->DeltaEta * jet->DeltaEta + jet->DeltaPhi * jet->DeltaPhi);
        if (dR > DELTA_ETA_PHI_CUTOFF) continue;
        if (jet->PT > max_pt) {
            maxPTJet = jet;
        }
    }
    if (!maxPTJet) cerr << "Warning: Couldn't find hadronic jet.\n";
    return maxPTJet;
}

Float_t JetPairInvariantMass(Jet *jet1, Jet *jet2) {
    TLorentzVector *tlv1 = new TLorentzVector();
    tlv1->SetPtEtaPhiM(jet1->PT, jet1->Eta, jet1->Phi, jet1->Mass);
    TLorentzVector *tlv2 = new TLorentzVector();
    tlv2->SetPtEtaPhiM(jet2->PT, jet2->Eta, jet2->Phi, jet2->Mass);
    TLorentzVector tlv3 = *tlv1 + *tlv2;
    return tlv3.M();
}


TLorentzVector *ReconstructWW(Electron *electron, Muon *muon, Jet *hadronicJet, MissingET *missingET) {
    assert(!(electron && muon));
    assert(electron || muon);
    TLorentzVector *METVector = new TLorentzVector();
    METVector->SetPtEtaPhiE(missingET->MET, missingET->Eta, missingET->Phi, missingET->MET);
    TLorentzVector *leptonVector = new TLorentzVector();
    if (electron) {
        leptonVector->SetPtEtaPhiM(electron->PT, electron->Eta, electron->Phi, ELECTRON_MASS);
    }
    else {
        leptonVector->SetPtEtaPhiM(muon->PT, muon->Eta, muon->Phi, MUON_MASS);
    }

    TLorentzVector *neutrinoVector = ReconstructNeutrino(METVector, leptonVector);
    
    TLorentzVector *leptonicWVector = new TLorentzVector(*neutrinoVector + *leptonVector);

    TLorentzVector *hadronicWVector = new TLorentzVector();
    hadronicWVector->SetPtEtaPhiM(hadronicJet->PT, hadronicJet->Eta, hadronicJet->Phi, hadronicJet->Mass);
    TLorentzVector *WWVector = new TLorentzVector(*leptonicWVector + *hadronicWVector);
    return WWVector;

}

// Calculate the transverse momentum of the neutrino in a WW-scattering event. This is based on code from
// from http://acode-browser2.usatlas.bnl.gov/lxr-rel17/source/atlas/PhysicsAnalysis/AnalysisCommon/SpecialUtils/SpecialUtils/NeutrinoUtils.icc
TLorentzVector *ReconstructNeutrino(TLorentzVector *METVector, TLorentzVector *leptonVector) {
    double ptMiss = METVector->Pt();
    double alpha = pow(W_MASS, 2) + pow((METVector->Px() + leptonVector->Px()), 2) 
        + pow((METVector->Py() + leptonVector->Py()), 2) - pow(leptonVector->E(), 2);

    double beta = 0.5 * (alpha - pow(ptMiss, 2) + pow(leptonVector->Pz(), 2) );
    double gamma = -1*(beta*beta - ( pow(leptonVector->E(), 2) * pow(ptMiss, 2) ) ) /
        ( pow(leptonVector->E(), 2) - pow(leptonVector->Pz(), 2) );

    double lambda = 2 * beta * leptonVector->Pz() / ( pow(leptonVector->E(), 2) - pow(leptonVector->Pz(), 2) );
    double delta = pow(lambda, 2) - 4*gamma;

    if (delta < 0) {
        cerr << "Warning: Negative discriminant when reconstructing neutrino.\n";
        return NULL;
    }
    delta = sqrt(delta);
    double pz = (lambda - delta)/2.0;
    double e = sqrt(METVector->Px()*METVector->Px() + METVector->Py()*METVector->Py() + pz*pz);

    TLorentzVector *neutrinoVector = new TLorentzVector();
    neutrinoVector->SetPxPyPzE(METVector->Px(), METVector->Py(), pz, e);

    return neutrinoVector;
}
