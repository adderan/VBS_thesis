#ifndef COMMON_H
#define COMMON_H
#include "classes/DelphesClasses.h"

#include "Classifiers.h"
#include "Rtypes.h"
#include "ExRootClasses.h"


/*----------------------- Values -------------------- */

/****** PDG IDs *********/
#define ELECTRON 11
#define MUON 13
#define MAX_QUARK 9 //quarks are 1-9
#define ELECTRON_NEUTRINO 12
#define MUON_NEUTRINO 14
#define GLUON 21
#define W_BOSON 24

/***** Physical Constants ******/

//All in GeV/c^2
#define MUON_MASS 0.105658
#define ELECTRON_MASS 0.000510
#define W_MASS 80.4


/******Parameters ********/

#define JET_MVA_CUTOFF 0.9

/******Names of things************/
#define WW_MASS_HISTOGRAM_NAME "WWMass"


//maximum value of sqrt(DELTA_ETA^2 + DELTA_PHI^2) for anti-kt 10 jets
#define DELTA_ETA_PHI_CUTOFF 1.0

//maximum value of sqrt(eta^2 + phi^2) between two tracks that correspond to the same particle
#define MAX_DELTA_R 0.6

//Maximum Eta of lepton in WW scattering event
#define LEPTON_ETA_CUTOFF 2.4



/*----------------------- Functions ------------------------*/ 

double deltaR(double eta1, double phi1, double eta2, double phi2);
double GetJetEnergy(Jet *jet);
void FindTagJetPair(JetClassifier *classifier, TClonesArray *jets, Jet **tagJet1, Jet **tagJet2);
bool FindLepton(TClonesArray *electrons, TClonesArray *muons, TLorentzVector *lepton);
TLorentzVector *FindHadronicJet(TClonesArray *jets);
Float_t JetPairInvariantMass(Jet *jet1, Jet *jet2);
TLorentzVector *ReconstructWW(TLorentzVector *lepton, TLorentzVector *hadronicJet, TLorentzVector *missingET);
TLorentzVector *ReconstructNeutrino(TLorentzVector *METVector, TLorentzVector *leptonVector);
TLorentzVector *ParticleToVector(TRootLHEFParticle *particle);
TLorentzVector *ReconstructNeutrinoAlternate(TLorentzVector *MET, TLorentzVector *lepton);
void MatchPartonWWScatteringEvent(TClonesArray *particles, TLorentzVector **lepton, TLorentzVector **neutrino,
        TLorentzVector **quark1, TLorentzVector **quark2, TLorentzVector **w1, TLorentzVector **w2);
#endif
