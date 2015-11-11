#ifndef TRAIN_JET_H
#define TRAIN_JET_H
#include "TObject.h"

class TrainJet: public TObject {
public:
    Double_t BetaStar;
    Double_t PT;
    Double_t AbsEta;
    Double_t Mass;
    Double_t Energy;
    //static TCompare *fgCompare; //!
    //const TCompare *GetCompare() const { return fgCompare; }
    TrainJet() {BetaStar = 0;}

    ClassDef(TrainJet, 1)
};

#endif
