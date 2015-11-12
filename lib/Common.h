#ifndef COMMON_H
#define COMMON_H
#include "classes/DelphesClasses.h"
#include "TrainJet.h"


/*----------------------- Constants -------------------- */

/*PDG IDs */
#define PROTON 2212
#define ELECTRON 11
#define MUON 13
#define MAX_QUARK 9 //quarks are 1-9

/*Parameters */
#define JET_MVA_CUTOFF 0.9

//maximum value of sqrt(eta^2 + phi^2) between two tracks that correspond to the same particle
#define MAX_DELTA_R 0.6


/*----------------------- Functions ------------------------*/ 

double deltaR(double eta1, double phi1, double eta2, double phi2);
double GetJetEnergy(Jet *jet);


#endif
