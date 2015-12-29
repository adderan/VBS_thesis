#include <getopt.h>
#include <iostream>
#include "Common.h"

#include "TH1F.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TChain.h"
#include "ExRootTreeReader.h"

#include "TCanvas.h"
#include "TLegend.h"
#include "TStyle.h"

TH1F *makeErrorHist(TLorentzVector *(*NeutrinoRecFunction)(TLorentzVector *tlvMET, 
            TLorentzVector *tlvLep), ExRootTreeReader *reader) {
    TClonesArray *particleBranch = reader->UseBranch("Particle");
    gStyle->SetOptStat(0);
    TH1F *pzError = new TH1F("PzError", "Neutrino Pz Error", 20, -100, 100);
    pzError->GetXaxis()->SetTitle("p_{z}^{truth} - p_{z}^{reconstructed} (GeV)");
    pzError->GetXaxis()->CenterTitle();

    for (int i = 0; i < 4000; i++) {
        reader->ReadEntry(i);
        TLorentzVector *lepton, *trueNeutrino, *quark1, *quark2, *w1, *w2;
        MatchPartonWWScatteringEvent(particleBranch, &lepton, &trueNeutrino, &quark1, &quark2, &w1, &w2);
        if (!(lepton && trueNeutrino && quark1 && quark2 && w1 && w2)) continue;

        TLorentzVector *MET = new TLorentzVector();
        MET->SetPxPyPzE(trueNeutrino->Px(), trueNeutrino->Py(), 0, trueNeutrino->Pt());
        TLorentzVector *recNeutrino = NeutrinoRecFunction(MET, lepton);
        if (!recNeutrino) continue;
        std::cerr << "True Pz: " << trueNeutrino->Pz() << " Reconstructed Pz: " 
            << recNeutrino->Pz() << "\n";
        double res = trueNeutrino->Pz() - recNeutrino->Pz();
        pzError->Fill(res);
        
    }
    return pzError;
}

int main(int argc, char **argv) {
    char *eventsFileNames[50];
    char *histogramFileName = NULL;
    int nEventsFiles = 0;
    int c;
    while(1) {
        int option_index = 0;
        static struct option long_options[] = {
            {"eventsFile", required_argument, 0, 'a'},
            {"histogram", required_argument, 0, 'b'},
        };
        c = getopt_long(argc, argv, "ab:", long_options, &option_index);
        if (c==-1)
            break;
        switch(c) {
            case 'a':
                eventsFileNames[nEventsFiles] = optarg;
                nEventsFiles++;
                break;
            case 'b':
                histogramFileName = optarg;
                break;
        }
    }

    TChain *chain = new TChain("LHEF");
    for (int i = 0; i < nEventsFiles; i++) {
        chain->Add(eventsFileNames[i]);
    }
    ExRootTreeReader *reader = new ExRootTreeReader(chain);
    TH1F *fitError = makeErrorHist(ReconstructNeutrino2, reader);
    fitError->SetFillColor(kRed);
    TH1F *exactError = makeErrorHist(ReconstructNeutrino3, reader);
    exactError->SetFillColor(kBlue);
    TH1F *metCorrectionError = makeErrorHist(ReconstructNeutrinoMETCorrection, reader);
    metCorrectionError->SetFillColor(kGreen);
    TH1F *metFitError = makeErrorHist(ReconstructNeutrinoMETFit, reader);
    metFitError->SetFillColor(kYellow);

    TCanvas *canvas = new TCanvas();
    //fitError->Draw();
    //exactError->Draw("same");
    metFitError->Draw("same");
    metCorrectionError->Draw("same");
    
    TLegend *leg = new TLegend(0.6,0.7,0.89,0.89);
    //leg->AddEntry(fitError, "MET correction by fit", "f");
    //leg->AddEntry(exactError, "No MET correction", "f");
    leg->AddEntry(metFitError, "MET correction by fit", "f");
    leg->AddEntry(metCorrectionError, "MET correction by quadratic solution", "f");
    //leg->AddEntry(defaultError, "Default", "l");
    leg->Draw();

    TFile *outputFile = new TFile(histogramFileName, "RECREATE");
    canvas->Write();
    outputFile->Close();

}
