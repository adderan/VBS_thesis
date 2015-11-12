#include "Common.h"
#include "TLorentzVector.h"
#include "ExRootClasses.h"
#include "classes/DelphesClasses.h"


double deltaR(double eta1, double phi1, double eta2, double phi2) {
    return sqrt((eta1 - eta2)*(eta1 - eta2) + (phi1 - phi2)*(phi1 - phi2));
}


double getEnergy1(double pt, double eta, double phi, double m) {
    TLorentzVector *tlv = new TLorentzVector();
    tlv->SetPtEtaPhiM(pt, eta, phi, m);
    return tlv->E();
}

void CopyFromJet(TrainJet *trainJet, Jet *jet) {

    trainJet->BetaStar = jet->BetaStar;
    trainJet->AbsEta = abs(jet->Eta);
    trainJet->PT = jet->PT;
    trainJet->Mass = jet->Mass;
    TLorentzVector *tlv = new TLorentzVector();
    tlv->SetPtEtaPhiM(jet->PT, jet->Eta, jet->Phi, jet->Mass);
    trainJet->Energy = tlv->E();
}
