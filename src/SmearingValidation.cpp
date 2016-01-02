#include <iostream>
#include <math.h>
#include <getopt.h>

#include "TProfile.h"
#include "ExRootTreeReader.h"
#include "classes/DelphesClasses.h"
#include "ExRootClasses.h"
#include "TClonesArray.h"
#include "TCanvas.h"
#include "Common.h"
#include "TRandom.h"
#include "THStack.h"
#include "TLegend.h"

double sigma_ms(double pt, double eta) {
    double ms[2][3] = 
    {
        {0.24, 0.02676, 0.00012},
        {0.0, 0.0388, 0.00016}
    };
    int range;
    if (abs(eta) < 1.05) {
        range = 0;
    }
    else {
        range = 1;
    }
    double res = sqrt(pow(ms[range][0]/pt, 2) + pow(ms[range][1], 2) + pow(ms[range][2]*pt, 2));
    return res;

}

double sigma_id(double pt, double eta) {
    double id[15][2] = 
    {
        {0.01061, 0.000157},
        {0.01084, 0.000153},
        {0.01124, 0.000150},
        {0.01173, 0.000149},
        {0.01269, 0.000148},
        {0.01406, 0.000161},
        {0.01623, 0.000192},
        {0.01755, 0.000199},
        {0.01997, 0.000232},
        {0.02453, 0.000261},
        {0.03121, 0.000297},
        {0.03858, 0.000375},
        {0.05273, 0.000465},
        {0.05329, 0.000642},
        {0.05683, 0.000746}
    };

    double eta_step = 0.18;
    int i = (int)(abs(eta)/eta_step);
    if (i > 14) i = 14;
    double res = sqrt(id[i][0]*id[i][0] + id[i][1]*id[i][1]*pt*pt);
    return res;
}

double sigma_electron(double E, double eta) {
    double eres[2][3] = 
    {
        {0.3, 0.010, 0.010},
        {0.3, 0.15, 0.015}
    };
    int i;
    if (abs(eta) < 1.4) {
        i = 0;
    }
    else {
        i = 1;
    }
    double res = sqrt(eres[i][0]*eres[i][0] + eres[i][1]*eres[i][1]*E + eres[i][2]*eres[i][2]*E*E);
    return res;
}

GenParticle *find_corresponding_electron(Electron *electron, TClonesArray *particles) {
    for (int i = 0; i < particles->GetEntriesFast(); i++) {
        GenParticle *particle = (GenParticle*)particles->At(i);
        if (!(abs(particle->PID) == 11)) continue;
        double dphi = electron->Phi - particle->Phi;
        double dEta = electron->Eta - particle->Eta;
        if (dphi*dphi + dEta*dEta > 0.3) continue;
        return particle;
    }
    return NULL;
}

GenParticle *find_corresponding_muon(Muon *muon, TClonesArray *particles) {
    for (int i = 0; i < particles->GetEntriesFast(); i++) {
        GenParticle *particle = (GenParticle*)particles->At(i);
        if (!(abs(particle->PID) == 13)) continue;
        double dphi = muon->Phi - particle->Phi;
        double dEta = muon->Eta - particle->Eta;
        if (dphi*dphi + dEta*dEta > 0.1) continue;
        return particle;
    }
    return NULL;
}

int main(int argc, char **argv) {
    char *delphesFileName = NULL;
    char *plotName = NULL;
    char *resFunctionName = NULL;
    int c;
    while(1) {
        int option_index = 0;
        static struct option long_options[] = {
            {"delphes", required_argument, 0, 'a'},
            {"plot", required_argument, 0, 'b'},
            {"resFunction", required_argument, 0, 'c'}
        };
        c = getopt_long(argc, argv, "abc:", long_options, &option_index);
        if (c==-1)
            break;
        switch(c) {
            case 'a':
                delphesFileName = optarg;
                break;
            case 'b':
                plotName = optarg;
                break;
            case 'c':
                resFunctionName = optarg;
                break;
        }
    }
    TFile *delphesFile = new TFile(delphesFileName);
    TTree *delphes = (TTree*)delphesFile->Get("Delphes");
    ExRootTreeReader *reader = new ExRootTreeReader(delphes);


    double (*resFunction)(double, double);
    TClonesArray *particleBranch = reader->UseBranch("Particle");
    TClonesArray *electronBranch = reader->UseBranch("Electron");
    TClonesArray *muonBranch = reader->UseBranch("Muon");
    enum {ELECTRON_MODE, MUON_MODE} mode;
    THStack *stack = new THStack("resolution", "resolution");

    if (strcmp(resFunctionName, "ms") == 0) {
        resFunction = sigma_ms;
        mode = MUON_MODE;
    }
    else if (strcmp(resFunctionName, "id") == 0) {
        resFunction = sigma_id;
        mode = MUON_MODE;
    }
    else if (strcmp(resFunctionName, "electron") == 0) {
        resFunction = sigma_electron;
        mode = ELECTRON_MODE;
    }
    TRandom *rand = new TRandom();

    TProfile *delphesResolution = new TProfile("DelphesSmearing", "Delphes Smearing", 100, 0, 50, 0, 1);
    TProfile *validationResolution = new TProfile("DirectSmearing", "Direct Smearing", 100, 0, 50, 0, 1);
    for (int i = 0; i < reader->GetEntries(); i++) {
        reader->ReadEntry(i);
        if (mode == ELECTRON_MODE) {
            if (electronBranch->GetEntriesFast() == 0) {
                continue;
            }
            Electron *smeared_electron = (Electron*)electronBranch->At(0);
            GenParticle *truth_electron = find_corresponding_electron(smeared_electron, particleBranch);
            if (!truth_electron) {
                continue;
            }

            TLorentzVector *smeared_tlv = new TLorentzVector();
            smeared_tlv->SetPtEtaPhiM(smeared_electron->PT, smeared_electron->Eta, smeared_electron->Phi, ELECTRON_MASS);
            double delphesResidual = pow(truth_electron->E - smeared_tlv->E(), 2);
            double trueResolution = resFunction(truth_electron->E, truth_electron->Eta);
            double validationResidual = pow(truth_electron->E - rand->Gaus(truth_electron->E, 
                        trueResolution), 2);
            delphesResolution->Fill(truth_electron->E, delphesResidual);
            validationResolution->Fill(truth_electron->E, validationResidual);
            std::cerr << "Simulation residual = " << validationResidual << "\n";
        }
        else if (mode == MUON_MODE) {
            if (muonBranch->GetEntriesFast() == 0) continue;
            Muon *smeared_muon = (Muon*)muonBranch->At(0);
            GenParticle *truth_muon = find_corresponding_muon(smeared_muon, particleBranch);
            if (!truth_muon) continue;
            double delphesResidual = pow(truth_muon->PT - smeared_muon->PT, 2);
            std::cerr << "Residual = " << delphesResidual << "\n";
            double trueResolution = resFunction(truth_muon->PT, truth_muon->Eta);
            double validationResidual = pow(truth_muon->PT - rand->Gaus(truth_muon->PT, 
                        truth_muon->PT*trueResolution), 2);
            delphesResolution->Fill(truth_muon->PT, delphesResidual);
            validationResolution->Fill(truth_muon->PT, validationResidual);
            std::cerr << "Simulation residual = " << validationResidual << "\n";
        }
    }
    TCanvas *canvas = new TCanvas();
    delphesResolution->SetLineColor(kRed);
    validationResolution->SetLineColor(kBlue);
    stack->Add(delphesResolution);
    stack->Add(validationResolution);
    TLegend *legend = new TLegend(0.6,0.7,0.89,0.89);
    legend->AddEntry(delphesResolution, "Delphes Smearing", "l");
    legend->AddEntry(validationResolution, "Direct Smearing", "l");

    stack->Draw();
    legend->Draw("same");

    if (mode == MUON_MODE) {
        stack->SetTitle("Muon Resolution");
        stack->GetXaxis()->SetTitle("p_{t} (GeV/c)");
        stack->GetYaxis()->SetTitle("(Pt_{truth} - Pt_{smeared})^{2}");
        stack->GetXaxis()->CenterTitle();
        stack->GetYaxis()->CenterTitle();

    }
    if (mode == ELECTRON_MODE) {
        stack->SetTitle("Electron Resolution");
        stack->GetXaxis()->SetTitle("E (GeV/c^{2})");
        stack->GetYaxis()->SetTitle("(E_{truth} - E_{smeared})^{2}");
        stack->GetXaxis()->CenterTitle();
        stack->GetYaxis()->CenterTitle();
    }
    TFile *output = new TFile(plotName, "RECREATE");
    canvas->Write();
    output->Close();
}
