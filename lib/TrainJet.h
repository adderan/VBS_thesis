#ifndef TRAIN_JET_H
#define TRAIN_JET_H
#include "TObject.h"

class TrainJet: public TObject {
public:
    Float_t BetaStar;
    Float_t PT;
    Float_t AbsEta;
    Float_t Mass;
    Float_t Energy;
    //static TCompare *fgCompare; //!
    //const TCompare *GetCompare() const { return fgCompare; }
    TrainJet() {BetaStar = 0;}

    ClassDef(TrainJet, 1)
};


#endif
