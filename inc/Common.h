#ifndef COMMON_H
#define COMMON_H



/*----------------------- Constants -------------------- */

/*PDG IDs */
#define PROTON 2212
#define ELECTRON 11
#define MUON 13
#define MAX_QUARK 9 //quarks are 1-9

/*Parameters */

//maximum value of sqrt(eta^2 + phi^2) between two tracks that correspond to the same particle
#define MAX_ETA_PHI 0.1 


/*----------------------- Functions ------------------------*/ 

double etaPhi(double eta1, double phi1, double eta2, double phi2);
double getEnergy1(double pt, double eta, double phi, double m);
#endif
