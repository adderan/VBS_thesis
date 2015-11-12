#ifndef CLASSIFIERS_H
#define CLASSIFIERS_H

#include "TMVA/Reader.h"
#include "classes/DelphesClasses.h"
#include "TrainJet.h"

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

#endif
