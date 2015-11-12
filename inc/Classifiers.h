#ifndef CLASSIFIERS_H
#define CLASSIFIERS_H

#include "TMVA/Reader.h"
#include "classes/DelphesClasses.h"
#include "TrainJet.h"

class JetClassifier {
public:
    TrainJet *CurrentJet;

    TMVA::Reader *reader;

    JetClassifier(char *weightsFileName);
    bool isTaggingJet(Jet *jet);
};

#endif
