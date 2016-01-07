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
    if (AbsEta > JVF_CUTOFF) {
        BetaStar = 1.0;
    }
    else {
        BetaStar = jet->BetaStar;
    }

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

    reader->BookMVA("LD", weightsFileName);
}

Double_t EventClassifier::ScoreEvent(struct WWScatteringComponents *event) {
    TLorentzVector *jetPair = new TLorentzVector(*event->positiveJet + *event->negativeJet);
    HadronicJetAbsEta = abs(event->hadronicJet->Eta());
    HadronicJetPT = event->hadronicJet->Phi();
    HadronicJetMass = event->hadronicJet->M();
    MissingET = event->missingET->E();
    Mjj = jetPair->M();
    LeptonAbsEta = abs(event->lepton->Eta());
    LeptonPT = event->lepton->Pt();

    return reader->EvaluateMVA("LD");
}

