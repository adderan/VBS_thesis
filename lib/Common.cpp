#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "Common.h"
#include "TLorentzVector.h"
#include "TClonesArray.h"
#include "TMinuit.h"

#include "ExRootClasses.h"
#include "classes/DelphesClasses.h"

#include "Classifiers.h"

using namespace std;

//Find the separation between two points in Eta-phi space
double deltaR(double eta1, double phi1, double eta2, double phi2) {
    return sqrt((eta1 - eta2)*(eta1 - eta2) + (phi1 - phi2)*(phi1 - phi2));
}

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

//Match a WW Scattering event from a Delphes Tree
WWScatteringComponents::WWScatteringComponents(JetClassifier *jetClassifier, 
        TClonesArray *electronBranch, 
        TClonesArray *muonBranch, TClonesArray *jetBranch, TClonesArray *ETBranch) {
    isGoodEvent = true;
    MissingET *METParticle = (MissingET*)ETBranch->At(0);
    if (!(METParticle->MET > 0.0)) isGoodEvent = false;
    missingET = new TLorentzVector();
    missingET->SetPtEtaPhiE(METParticle->MET, METParticle->Eta, METParticle->Phi, METParticle->MET);

    positiveJet = new TLorentzVector();
    negativeJet = new TLorentzVector();
    bool found_tag_jets = FindTagJetPair(jetClassifier, jetBranch, positiveJet, negativeJet);

    lepton = new TLorentzVector();
    bool found_lepton = FindLepton(electronBranch, muonBranch, lepton);

    hadronicJet = FindHadronicJet(jetBranch);
    if(!(found_tag_jets && found_lepton && hadronicJet)) isGoodEvent = false;
}

//Finds the two tagging high-eta tagging jets on opposite eta hemispheres, 
//using the provided tagging jet classifier
bool FindTagJetPair(JetClassifier *classifier, TClonesArray *jets, 
        TLorentzVector *positiveJet, TLorentzVector *negativeJet) {
    int nJets = jets->GetEntriesFast();
    bool found_positive_jet = false;
    bool found_negative_jet = false;
    for(int i = 0; i < nJets; i++) {
        if (found_positive_jet && found_negative_jet) return true;
        Jet *jet = (Jet*)jets->At(i);
        if (classifier->isTaggingJet(jet)) {
            if (!found_positive_jet && jet->Eta > 0) {
                positiveJet->SetPtEtaPhiM(jet->PT, jet->Eta, jet->Phi, jet->Mass);
                found_positive_jet = true;
            }
            else if (!found_negative_jet && jet->Eta < 0) {
                negativeJet->SetPtEtaPhiM(jet->PT, jet->Eta, jet->Phi, jet->Mass);
                found_negative_jet = true;
            }
        }
    }
    return (found_positive_jet && found_negative_jet);
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
        if (electron->PT < LEPTON_PT_CUTOFF) continue;

        if (electron->PT > max_pt) {
            max_pt = electron->PT;
            lepton->SetPtEtaPhiM(electron->PT, electron->Eta, electron->Phi, ELECTRON_MASS);
            found_lepton = true;
        }
    }
    for (int i = 0; i < nMuons; i++) {
        Muon *muon = (Muon*)muons->At(i);
        if (abs(muon->Eta) < LEPTON_ETA_CUTOFF) continue;
        if (muon->PT < LEPTON_PT_CUTOFF) continue;

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
        if (dR > DELTA_ETA_PHI_CUTOFF) {
            continue;
        }
        if (jet->PT > max_pt) {
            max_pt = jet->PT;
            maxPTJet = jet;
        }
    }
    if (!maxPTJet) {
        return NULL;
    }
    if (maxPTJet->BTag != 0) return NULL;

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

/* Find the components of a WW-scattering event at the parton level,
 */
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

TLorentzVector *ReconstructWW(TLorentzVector *lepton, TLorentzVector *hadronicJet, 
        TLorentzVector *missingET) {
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
TLorentzVector *ReconstructNeutrino(TLorentzVector *MET, TLorentzVector *lepton) {
    double pxMiss = MET->Px();
    double pyMiss = MET->Py();
    double ptMiss = MET->Pt();
    double alpha = pow(W_MASS,2)+pow((pxMiss+lepton->Px()),2)+pow((pyMiss+lepton->Py()),2) -
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

void delta2_fcn(int& npar, double *gout, double& result, double par[], int flag) {
    double delta2 = 0;
    double alpha = par[0];
    double r = par[1];
    double dphi = par[2];
    double l_pt = par[3];
    double l_m = par[4];
    double n_px = par[5];
    double n_py = par[6];

    r /= sqrt(l_pt*l_pt +l_m*l_m) - l_pt*cos(dphi + alpha);
    TLorentzVector *neut = new TLorentzVector();
    neut->SetPxPyPzE(n_px, n_py, 0.0, 0.0);
    neut->SetE(neut->P());
    TLorentzVector *neut_new = new TLorentzVector();
    neut_new->SetPxPyPzE(r*neut->P()*cos(neut->Phi()+alpha), 
            r*neut->P()*sin(neut->Phi()+alpha), 0.0, 0.0);
    neut_new->SetE(neut_new->P());
    delta2 = pow(neut_new->Px() - neut->Px(), 2) + pow(neut_new->Py() - neut->Py(), 2);

    result = delta2;
}

double FitAlpha(TLorentzVector *tlvMET, TLorentzVector *tlvLep) {

    TMinuit *fit = new TMinuit(7);
    string par_name[7] = {"alpha", "r", "dphi", "l_pt", "l_m", "n_px", "n_py"};
	double par_ival[7] = {0.0, (W_MASS*W_MASS - tlvLep->M()*tlvLep->M())/(2*tlvMET->Pt()), tlvMET->Phi()-tlvLep->Phi(), tlvLep->Pt(), tlvLep->M(), tlvMET->Px(), tlvMET->Py()};
	double par_step[7] = {0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	double par_min[7] = {-3.15, 0.0, -3.15, 0.0, 0.0, -10000.0, -10000.0};
	double par_max[7] = {3.15, 1.0, 3.15, 10000.0, 80.0, 10000.0, 10000.0};
    for(int i = 0; i < 7; i++) {
        fit->DefineParameter(i, par_name[i].c_str(), par_ival[i], par_step[i], par_min[i], par_max[i]);
        if (i != 0) fit->FixParameter(i);
    }
    double a = 1.0;
	double e_a = 1.0;
		
	fit->Migrad();
		
	double ret = fit->GetParameter(0, a, e_a);

    if (ret > 0.0) return a;
    else {
        return 0.0;
    }
}

TLorentzVector *ReconstructNeutrino2(TLorentzVector *tlvMET, TLorentzVector *tlvLep) {
    TLorentzVector *tlvrej = new TLorentzVector();
    TLorentzVector *tlvNu = new TLorentzVector();
    //double mtw = sqrt(2*tlvLep->Pt()*tlvMET->Pt()*(1-cos(tlvLep->Phi() - tlvMET->Phi())));
    double c1 = pow(W_MASS,2) - pow(tlvLep->M(),2) + 
        2*(tlvLep->Px()*tlvMET->Px()+tlvLep->Py()*tlvMET->Py());
    double b1 = 2*tlvLep->Pz();
    double A = 4*pow(tlvLep->E(),2) - pow(b1, 2);
    double B = -2*c1*b1;
    double C = 4*pow(tlvLep->E(),2) * pow(tlvMET->Pt(),2) - pow(c1,2);
    double discr = B*B - 4*A*C;
    if (discr > 0) {
        double sol1 = (-1*B + sqrt(discr))/(2*A);
        tlvrej->SetPtEtaPhiE( tlvMET->Pt(), tlvMET->Eta(), tlvMET->Phi(), tlvMET->E() );
        double sol2 = (-1*B - sqrt(discr))/(2*A);

        double pz;
        if (abs(sol1) > abs(sol2)) {
            pz = sol2;
            tlvrej->SetPz(sol1);
            tlvrej->SetE(tlvrej->P());
        }
        else {
            pz = sol1;
			tlvrej->SetPz(sol2);
			tlvrej->SetE(tlvrej->P());
        }
        tlvMET->SetPz(pz);
		tlvMET->SetE(tlvMET->P());
        tlvNu->SetPtEtaPhiE(tlvMET->Pt(), tlvMET->Eta(), tlvMET->Phi(), tlvMET->E());
    }
    else {
        std::cerr << "Attempting to deal with negative discriminant.\n";
        double alpha = FitAlpha(tlvMET, tlvLep);
        double dphi = tlvMET->Phi()-tlvLep->Phi();
		double r = (W_MASS*W_MASS - tlvLep->M()*tlvLep->M())/(2*tlvMET->Pt()*(sqrt(tlvLep->Pt()*tlvLep->Pt()+tlvLep->M()*tlvLep->M())-tlvLep->Pt()*cos(dphi+alpha)));
		double old_p = tlvMET->P();
		double old_phi = tlvMET->Phi();
		tlvMET->SetPx(r*old_p*cos(old_phi+alpha));
		tlvMET->SetPy(r*old_p*sin(old_phi+alpha));
		double c1 = W_MASS*W_MASS - pow(tlvLep->M(),2) + 
            2*(tlvLep->Px()*tlvMET->Px()+tlvLep->Py()*tlvMET->Py());
		double B = -2*c1*b1;
		//double C = 4*pow(tlvLep->E(),2)*pow(tlvMET->Pt(),2) - c1*c1;
	    //double discr = B*B-4*A*C;
		double pz = -1*B/(2*A);
		
		tlvMET->SetPz(pz);
		tlvMET->SetE(tlvMET->P());
        tlvNu->SetPtEtaPhiE(tlvMET->Pt(), tlvMET->Eta(), tlvMET->Phi(), tlvMET->E());
    }
    return tlvNu;
}

TLorentzVector *ReconstructNeutrino3(TLorentzVector *tlvMET, TLorentzVector *tlvLep) {
    double pzl = tlvLep->Pz();
    double Mw = W_MASS;
    double Ml = tlvLep->M();
    double pxl = tlvLep->Px();
    double pyl = tlvLep->Py();
    double pxn = tlvMET->Px();
    double pyn = tlvMET->Py();
    double El = tlvLep->E();
    double EMiss = tlvMET->Pt();

    double b = El;
    double c = EMiss*EMiss;
    double f = -pxl*pxn - pyl*pyn - (Mw*Mw - Ml*Ml)/2.0;
    double g = pzl;

    double discr = -pow(b,4)*c + b*b*c*g*g + b*b*f*f;
    if (discr > 0) {
        TLorentzVector *neutrino = new TLorentzVector();
        double sol1 = (-f*g - sqrt(discr))/(b*b - g*g);
        double sol2 = (sqrt(discr) - f*g)/(b*b - g*g);
        double pzn = abs(sol2) > abs(sol1) ? sol1 : sol2;
        std::cerr << "Pz = " << pzn << "\n";
        neutrino->SetPxPyPzE(pxn, pyn, pzn, EMiss);
        return neutrino;
    }
    else {
        return NULL;
    }
}

TLorentzVector *ReconstructNeutrinoMETCorrection(TLorentzVector *tlvMET, TLorentzVector *tlvLep) {
    double pzl = tlvLep->Pz();
    double Mw = W_MASS;
    double Ml = tlvLep->M();
    double pxl = tlvLep->Px();
    double pyl = tlvLep->Py();
    double pxn = tlvMET->Px();
    double pyn = tlvMET->Py();
    double El = tlvLep->E();
    double phi = tlvMET->Phi();
    double MET = tlvMET->Pt();

    double a = pxl*cos(phi);
    double b = pyl*sin(phi);
    double c = (Mw*Mw - Ml*Ml)/2.0;
    double d = -El*El + pzl*pzl;

    double alpha = a + b;
    double discr = 4*c*c*alpha*alpha - 4*c*c*(alpha*alpha + d);

    if (discr < 0) return NULL;
    double METPrime = (-2*c*alpha + sqrt(discr))/(2*(alpha*alpha + d));

    TLorentzVector *tlvMETPrime = new TLorentzVector();
    tlvMETPrime->SetPtEtaPhiE(METPrime, 0.0, phi, METPrime);

    pxn = tlvMETPrime->Px();
    pyn = tlvMETPrime->Py();

    double pzn = -pzl*(pxl*pxn + pyl*pyn + c)/(El*El - pzl*pzl);
    TLorentzVector *neutrino = new TLorentzVector();
    neutrino->SetPxPyPzE(pxn, pyn, pzn, MET);
    return neutrino;

}

TLorentzVector *ReconstructNeutrinoMETFit(TLorentzVector *tlvMET, TLorentzVector *tlvLep) {
    TLorentzVector *tlvNu = new TLorentzVector();
    //double mtw = sqrt(2*tlvLep->Pt()*tlvMET->Pt()*(1-cos(tlvLep->Phi() - tlvMET->Phi())));
    double b1 = 2*tlvLep->Pz();
    double A = 4*pow(tlvLep->E(),2) - pow(b1, 2);
    double alpha = FitAlpha(tlvMET, tlvLep);
    double dphi = tlvMET->Phi()-tlvLep->Phi();
    double r = (W_MASS*W_MASS - tlvLep->M()*tlvLep->M())/(2*tlvMET->Pt()*(sqrt(tlvLep->Pt()*tlvLep->Pt()+tlvLep->M()*tlvLep->M())-tlvLep->Pt()*cos(dphi+alpha)));
    double old_p = tlvMET->P();
    double old_phi = tlvMET->Phi();
    tlvMET->SetPx(r*old_p*cos(old_phi+alpha));
    tlvMET->SetPy(r*old_p*sin(old_phi+alpha));
    double c1 = W_MASS*W_MASS - pow(tlvLep->M(),2) + 
        2*(tlvLep->Px()*tlvMET->Px()+tlvLep->Py()*tlvMET->Py());
    double B = -2*c1*b1;
    //double C = 4*pow(tlvLep->E(),2)*pow(tlvMET->Pt(),2) - c1*c1;
    //double discr = B*B-4*A*C;
    double pz = -1*B/(2*A);
    
    tlvMET->SetPz(pz);
    tlvMET->SetE(tlvMET->P());
    tlvNu->SetPtEtaPhiE(tlvMET->Pt(), tlvMET->Eta(), tlvMET->Phi(), tlvMET->E());
    return tlvNu;
}

