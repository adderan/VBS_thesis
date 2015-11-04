#include "Common.h"
#include "TLorentzVector.h"


double etaPhi(double eta1, double phi1, double eta2, double phi2) {
    return sqrt((eta1 - eta2)*(eta1 - eta2) + (phi1 - phi2)*(phi1 - phi2));
}


double getEnergy1(double pt, double eta, double phi, double m) {
    TLorentzVector *tlv = new TLorentzVector();
    tlv->SetPtEtaPhiM(pt, eta, phi, m);
    return tlv->E();
}
