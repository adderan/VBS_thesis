#include "Common.h"
#include "TLorentzVector.h"
#include "ExRootClasses.h"
#include "classes/DelphesClasses.h"


double deltaR(double eta1, double phi1, double eta2, double phi2) {
    return sqrt((eta1 - eta2)*(eta1 - eta2) + (phi1 - phi2)*(phi1 - phi2));
}


double GetJetEnergy(Jet *jet) {
    TLorentzVector *tlv = new TLorentzVector();
    tlv->SetPtEtaPhiM(jet->PT, jet->Eta, jet->Phi, jet->Mass);
    return tlv->E();
}

