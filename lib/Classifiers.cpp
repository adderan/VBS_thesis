#include "math.h"

#include "Classifiers.h"
#include "Common.h"

JetClassifier::JetClassifier(char *weightsFileName) {
    reader = new TMVA::Reader();
    reader->AddVariable("TrainJet.PT", &PT);
    reader->AddVariable("TrainJet.AbsEta", &AbsEta);
    reader->AddVariable("TrainJet.Mass", &Mass);
    reader->AddVariable("TrainJet.BetaStar", &BetaStar);
    reader->AddVariable("TrainJet.Energy", &Energy);
    
    reader->BookMVA("BDT", weightsFileName);
}

bool JetClassifier::isTaggingJet(Jet *jet) {
    PT = jet->PT;
    AbsEta = abs(jet->Eta);
    Mass = jet->Mass;
    Energy = GetJetEnergy(jet);
    BetaStar = jet->BetaStar;

    Double_t mva_value = reader->EvaluateMVA("BDT");
    return (mva_value > JET_MVA_CUTOFF);
}

EventClassifier::EventClassifier(char *weightsFileName) {
    reader = new TMVA::Reader();
    reader->AddVariable("WWScatteringEvent.HadronicJetAbsEta", &HadronicJetAbsEta);
    reader->AddVariable("WWScatteringEvent.HadronicJetPT", &HadronicJetPT);
    reader->AddVariable("WWScatteringEvent.HadronicJetMass", &HadronicJetMass);
    reader->AddVariable("WWScatteringEvent.MissingET", &MissingET);
    reader->AddVariable("WWScatteringEvent.Mjj", &Mjj);
    reader->AddVariable("WWScatteringEvent.LeptonAbsEta", &LeptonAbsEta);
    reader->AddVariable("WWScatteringEvent.LeptonPT", &LeptonPT);

    reader->BookMVA("BDT", weightsFileName);
}

Double_t EventClassifier::ScoreEvent(TLorentzVector *positiveJet, TLorentzVector *negativeJet, TLorentzVector *lepton, 
        TLorentzVector *hadronicJet, Float_t MET) {
    TLorentzVector *jetPair = new TLorentzVector(*positiveJet + *negativeJet);
    HadronicJetAbsEta = abs(hadronicJet->Eta());
    HadronicJetPT = hadronicJet->Phi();
    HadronicJetMass = hadronicJet->M();
    MissingET = MET;
    Mjj = jetPair->M();
    LeptonAbsEta = abs(lepton->Eta());
    LeptonPT = lepton->Pt();

    return reader->EvaluateMVA("BDT");
}

