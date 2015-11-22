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

/*
bool FindWWComponents(TClonesArray *particles, TLorentzVector **lepton, TLorentzVector *MET, TLorentzVector *hadronicJet) {
    double max_lepton_pt, max_hadronic_jet_pt;
    bool found_positive_tag_jet, found_negative_tag_jet;
    TRootLHEFParticle *lepton, *hadronicJet;
    for (int i = 0; i < particles->GetEntriesFast(); i++) {
        TRootLHEFParticle *particle = (TRootLHEFParticle*)particles->At(i);
        if (particle->PID == ELECTRON || particle->PID == MUON) {
            if (particle->PT > max_lepton_pt) {
                lepton = particle;
                continue;
            }
        }
        if (particle->PID <= MAX_QUARK) {
            if (particle->PT > max_lepton_pt) {
                hadronicJet = particle;
                max_hadroic_jet_pt = particle->PT;
            }
        }
    }
}
*/

TLorentzVector *ParticleToVector(TRootLHEFParticle *particle) {
    TLorentzVector *tlv = new TLorentzVector();
    tlv->SetPtEtaPhiE(particle->PT, particle->Eta, particle->Phi, particle->E);
    return tlv;
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
bool FindLepton(TClonesArray *electrons, TClonesArray *muons, TLorentzVector *lepton) {
    int nElectrons = electrons->GetEntriesFast();
    int nMuons = muons->GetEntriesFast();
    Float_t max_pt = 0.0;

    bool found_lepton = false;
    for (int i = 0; i < nElectrons; i++) {
        Electron *electron = (Electron*)electrons->At(i);
        if(abs(electron->Eta) > LEPTON_ETA_CUTOFF) continue;

        if (electron->PT > max_pt) {
            max_pt = electron->PT;
            lepton->SetPtEtaPhiM(electron->PT, electron->Eta, electron->Phi, ELECTRON_MASS);
            found_lepton = true;
        }
    }
    for (int i = 0; i < nMuons; i++) {
        Muon *muon = (Muon*)muons->At(i);
        if (abs(muon->Eta) > LEPTON_ETA_CUTOFF) continue;

        if (muon->PT > max_pt) {
            max_pt = muon->PT;
            lepton->SetPtEtaPhiM(muon->PT, muon->Eta, muon->Phi, MUON_MASS);
            found_lepton = true;
        }
    }
    return found_lepton;
}

//Find the highest PT anti-kt 10 jet in the
//event
TLorentzVector *FindHadronicJet(TClonesArray *jets) {
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
    if (!maxPTJet) return NULL;
    TLorentzVector *jetVector = new TLorentzVector();
    jetVector->SetPtEtaPhiM(maxPTJet->PT, maxPTJet->Eta, maxPTJet->Phi, maxPTJet->Mass);
    return jetVector;
}

Float_t JetPairInvariantMass(Jet *jet1, Jet *jet2) {
    TLorentzVector *tlv1 = new TLorentzVector();
    tlv1->SetPtEtaPhiM(jet1->PT, jet1->Eta, jet1->Phi, jet1->Mass);
    TLorentzVector *tlv2 = new TLorentzVector();
    tlv2->SetPtEtaPhiM(jet2->PT, jet2->Eta, jet2->Phi, jet2->Mass);
    TLorentzVector tlv3 = *tlv1 + *tlv2;
    return tlv3.M();
}

void MatchPartonWWScatteringEvent(TClonesArray *particles, TLorentzVector **lepton, TLorentzVector **neutrino,
        TLorentzVector **quark1, TLorentzVector **quark2, TLorentzVector **w1, TLorentzVector **w2) {
    double min_abs_eta = 10000;
    for (int j = 0; j < particles->GetEntriesFast(); j++) {
        TRootLHEFParticle *particle = (TRootLHEFParticle*)particles->At(j);

        if (particle->Status == -1) continue;

        TRootLHEFParticle *mother = NULL;
        if(particle->Mother1 > 0) {
            mother = (TRootLHEFParticle*)particles->At(particle->Mother1 - 1);
        }

        if (particle->PID == 24) {
            *w1 = ParticleToVector(particle);
        }
        else if (particle->PID == -24) {
            *w2 = ParticleToVector(particle);
        }

        else if (abs(particle->PID) < MAX_QUARK || abs(particle->PID) == GLUON) {
            if (abs(particle->Eta) < min_abs_eta) {
                min_abs_eta = abs(particle->Eta);
                *quark1 = ParticleToVector(particle);
            }
        }
        else if (mother && abs(mother->PID) == W_BOSON) {
            if (abs(particle->PID) == ELECTRON_NEUTRINO || abs(particle->PID) == MUON_NEUTRINO) {
                *neutrino = ParticleToVector(particle);
            }
            if(abs(particle->PID) == ELECTRON || abs(particle->PT) == MUON) {
                *lepton = ParticleToVector(particle);
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
                *quark2 = ParticleToVector(particle);
            }
        }
    }
}

TLorentzVector *ReconstructWW(TLorentzVector *lepton, TLorentzVector *hadronicJet, TLorentzVector *missingET) {
    TLorentzVector *neutrino = ReconstructNeutrino(missingET, lepton);
    if (!neutrino) return NULL;
    
    TLorentzVector *leptonicW = new TLorentzVector(*neutrino + *lepton);

    TLorentzVector *hadronicW = new TLorentzVector();
    hadronicW->SetPtEtaPhiM(hadronicJet->Pt(), hadronicJet->Eta(), hadronicJet->Phi(), hadronicJet->M());
    TLorentzVector *WW = new TLorentzVector(*leptonicW + *hadronicW);
    //cerr << "WW mass: " << WWVector->M() << "\n";
    return WW;

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
    //cerr << "Neutrino Pz: " << pz << "\n";
    double e = sqrt(METVector->Px()*METVector->Px() + METVector->Py()*METVector->Py() + pz*pz);

    TLorentzVector *neutrinoVector = new TLorentzVector();
    neutrinoVector->SetPxPyPzE(METVector->Px(), METVector->Py(), pz, e);

    return neutrinoVector;
}

TLorentzVector *ReconstructNeutrinoAlternate(TLorentzVector *MET, TLorentzVector *lepton) {
    double pxMiss = MET->Px();
    double pyMiss = MET->Py();
    double ptMiss = MET->Pt();
    const double WMass = 80.4;
    double alpha = pow(WMass,2)+pow((pxMiss+lepton->Px()),2)+pow((pyMiss+lepton->Py()),2) -
        pow(lepton->E(),2);
    double beta = 0.5 * ( alpha-pow(ptMiss,2)+pow(lepton->Pz(),2) );
    double gamma = -( beta*beta - ( pow(lepton->E(),2)*pow(ptMiss,2) ) )
        / ( pow(lepton->E(),2)-pow(lepton->Pz(),2) );
    double lambda = 2*beta*lepton->Pz() / (pow(lepton->E(),2)-pow(lepton->Pz(),2));
    double delta = pow(lambda,2)-4*gamma;
    if (delta < 0) {
        return NULL;
    }
    delta = sqrt(delta);
    double pz = (lambda-delta)/2.0;
    double e  = sqrt(pxMiss*pxMiss+pyMiss*pyMiss+pz*pz);
    TLorentzVector *neutrino = new TLorentzVector();
    neutrino->SetPxPyPzE(pxMiss, pyMiss, pz, e);
    return neutrino;
}
