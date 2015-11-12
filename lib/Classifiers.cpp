#include "Classifiers.h"
#include "Common.h"

JetClassifier::JetClassifier(char *weightsFileName) {
    reader = new TMVA::Reader();
    reader->AddVariable("AbsEta", &(CurrentJet->AbsEta));
    reader->AddVariable("BetaStar", &(CurrentJet->BetaStar));
    reader->AddVariable("PT", &(CurrentJet->PT));
    reader->AddVariable("Mass", &(CurrentJet->Mass));
    reader->AddVariable("Energy", &(CurrentJet->Energy));
    
    reader->BookMVA("BDT", weightsFileName);
}

bool JetClassifier::isTaggingJet(Jet *jet) {
    CopyFromJet(CurrentJet, jet);
    Double_t mva_value = reader->EvaluateMVA("BDT");
    return (mva_value > JET_MVA_CUTOFF);
}

