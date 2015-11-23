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
