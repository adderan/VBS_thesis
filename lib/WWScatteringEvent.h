#ifndef WW_SCATTERING_EVENT_H
#define WW_SCATTERING_EVENT_H
#include "TObject.h"

class WWScatteringEvent: public TObject {
public:
    Float_t HadronicJetAbsEta;
    Float_t HadronicJetPT;
    Float_t HadronicJetMass;
    Float_t MissingET;
    Float_t Mjj;
    Float_t LeptonAbsEta;
    Float_t LeptonPT;
    WWScatteringEvent() {}

    ClassDef(WWScatteringEvent, 1);
};

#endif
