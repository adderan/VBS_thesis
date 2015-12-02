#ifndef CLASSIFIERS_H
#define CLASSIFIERS_H

#include "TMVA/Reader.h"
#include "classes/DelphesClasses.h"
#include "TLorentzVector.h"

class JetClassifier {
public:
    Float_t AbsEta;
    Float_t BetaStar;
    Float_t Mass;
    Float_t Energy;
    Float_t PT;

    TMVA::Reader *reader;

    JetClassifier(char *weightsFileName);
    bool isTaggingJet(Jet *jet);
};


class EventClassifier {
public:
    Float_t HadronicJetAbsEta;
    Float_t HadronicJetPT;
    Float_t HadronicJetMass;
    Float_t MissingET;
    Float_t Mjj;
    Float_t LeptonAbsEta;
    Float_t LeptonPT;

    TMVA::Reader *reader;
    
    EventClassifier(char *weightsFileName);
    Double_t ScoreEvent(struct WWScatteringComponents *event);
};

#endif
