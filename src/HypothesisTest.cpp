#include <getopt.h>
#include <iostream>

#include "RooWorkspace.h"
#include "RooStats/ProfileLikelihoodCalculator.h"
#include "TH1F.h"
#include "TFile.h"
#include "Common.h"
#include "RooArgList.h"
#include "RooAbsReal.h"
#include "RooStats/HypoTestResult.h"
#include "RooDataHist.h"
#include "RooFitResult.h"
#include "RooGaussian.h"
#include "RooAddPdf.h"
#include "RooAbsReal.h"
#include "RooStats/ModelConfig.h"
#include "RooPlot.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "RooExponential.h"
#include "RooStats/ProfileLikelihoodTestStat.h"

RooAddPdf *MakeModel(RooDataHist *data, RooRealVar *mww, char *name) {
    char *modelName = (char*)calloc(50, sizeof(char));
    strcat(modelName, name);
    strcat(modelName, "model");
    char *signalName = (char*)calloc(50, sizeof(char));
    char *backgroundName = (char*)calloc(50, sizeof(char));
    strcat(signalName, name);
    strcat(signalName, "signal");
    strcat(backgroundName, name);
    strcat(backgroundName, "background");

    char *meanName = (char*)calloc(50, sizeof(char));
    strcat(meanName, name);
    strcat(meanName, "mean");

    char *sigmaName = (char*)calloc(50, sizeof(char));
    strcat(sigmaName, name);
    strcat(sigmaName, "sigma");

    char *decayName = (char*)calloc(50, sizeof(char));
    strcat(decayName, name);
    strcat(decayName, "decay");

    char *weightName = (char*)calloc(50, sizeof(char));
    strcat(weightName, name);
    strcat(weightName, "weight");

    RooRealVar *mean = new RooRealVar(meanName, "mean", 0, 3000, "GeV");
    RooRealVar *sigma = new RooRealVar(sigmaName, "sigma", 0, 3000, "GeV");
    RooGaussian *signal = new RooGaussian(signalName, "signal", *mww, *mean, *sigma);

    RooRealVar *decay = new RooRealVar(decayName, "decay", -10, 10, "GeV");
    RooExponential *background = new RooExponential(backgroundName, "background", *mww, *decay);

    RooRealVar *weight = new RooRealVar("decayweight", "weight", 0.0, 1.0, "None");

    RooAddPdf *model = new RooAddPdf(modelName, "model", *signal, *background, *weight);

    model->fitTo(*data);
    mean->setConstant();
    sigma->setConstant();
    weight->setConstant();
    decay->setConstant();
    return model;
}

double RunHypoTest(char *smwwFileName, char *ttbarFileName, char *wp3jetsFileName, char *wp4jetsFileName, char *opsFileName, char *outputFileName) {
    TFile *smwwFile = new TFile(smwwFileName);
    TFile *ttbarFile = new TFile(ttbarFileName);
    TFile *wp3jetsFile = new TFile(wp3jetsFileName);
    TFile *wp4jetsFile = new TFile(wp4jetsFileName);
    TFile *opsFile = new TFile(opsFileName);

    TFile *outputFile = new TFile(outputFileName, "UPDATE");

    TH1F *smww = (TH1F*)smwwFile->Get(WW_MASS_HISTOGRAM_NAME);
    TH1F *ttbar = (TH1F*)ttbarFile->Get(WW_MASS_HISTOGRAM_NAME);
    TH1F *wp3jets = (TH1F*)wp3jetsFile->Get(WW_MASS_HISTOGRAM_NAME);
    TH1F *wp4jets = (TH1F*)wp4jetsFile->Get(WW_MASS_HISTOGRAM_NAME);

    //Histogram of ww-scattering with effective operator contributions
    TH1F *ops = (TH1F*)opsFile->Get(WW_MASS_HISTOGRAM_NAME);

    RooRealVar *mww = new RooRealVar("mww", "M_{WW}", 600, 2500, "GeV");

    RooDataHist smData("smData", "smData", RooArgList(*mww), smww);
    RooDataHist opsData("opsData", "opsData", RooArgList(*mww), ops);
    RooDataHist ttbarData("ttbarData", "ttbarData", RooArgList(*mww), ttbar);
    RooDataHist wp3jetsData("wp3jetsData", "wp3jetsData", RooArgList(*mww), wp3jets);
    RooDataHist wp4jetsData("wp4jetsData", "wp4jetsData", RooArgList(*mww), wp4jets);


    RooAddPdf *smModel = MakeModel(&smData, mww, (char*)"sm");

    RooAddPdf *opsModel = MakeModel(&opsData, mww, (char*)"ops");
    RooAddPdf *ttbarModel = MakeModel(&ttbarData, mww, (char*)"ttbar");
    RooAddPdf *wp3jetsModel = MakeModel(&wp3jetsData, mww, (char*)"wp3jets");
    RooAddPdf *wp4jetsModel = MakeModel(&wp4jetsData, mww, (char*)"wp4jets");

    TCanvas *canvas = new TCanvas(opsFileName);
    RooPlot *frame = mww->frame();
    frame->SetTitle("");
    //smData.plotOn(frame, RooFit::LineColor(kBlack), RooFit::Name("smData"));

    //smModel->plotOn(frame, RooFit::LineColor(kBlue), RooFit::Name("smModel"));
    //ttbarModel->plotOn(frame, RooFit::LineColor(kRed), RooFit::Name("ttbarModel"));
    //wp3jetsModel->plotOn(frame, RooFit::LineColor(kYellow), RooFit::Name("wpjetsModel"));
    opsData.plotOn(frame);
    opsModel->plotOn(frame, RooFit::LineColor(kBlue), RooFit::Name("opsModel"));
    TLegend *leg = new TLegend(0.65,0.73,0.86,0.87);
    //leg->AddEntry(frame->findObject("smModel"), "SM Model", "lep");
    //leg->AddEntry(frame->findObject("ttbarModel"), "TTBar Model", "lep");
    //leg->AddEntry(frame->findObject("wp3jetsModel"), "WP3Jets Model", "lep");
    //leg->AddEntry(frame->findObject("opsModel"), "Effective Operator Model", "lep");

    frame->Draw();
    leg->Draw();
    canvas->Write();

    Double_t ww_x = WW_CROSS_SECTION * smww->GetEntries();
    Double_t ttbar_x = TTBAR_CROSS_SECTION * ttbar->GetEntries();
    Double_t wp3jets_x = WP3JETS_CROSS_SECTION * wp3jets->GetEntries();
    Double_t wp4jets_x = WP4JETS_CROSS_SECTION * wp4jets->GetEntries();


    Double_t ttbar_weight = ttbar_x/(ttbar_x + wp3jets_x + wp4jets_x + ww_x);
    Double_t wp3jets_weight = wp3jets_x/(wp3jets_x + ttbar_x + ww_x);
    Double_t wp4jets_weight = wp4jets_x/(wp4jets_x + ttbar_x + ww_x);

    RooRealVar *ttbarWeight = new RooRealVar("ttbarWeight", "ttbarWeight", 0.0, 1.0, ttbar_weight);
    RooRealVar *wp3jetsWeight = new RooRealVar("wp3jetsWeight", "wp3jetsWeight", 0.0, 1.0, wp3jets_weight);
    RooRealVar *wp4jetsWeight = new RooRealVar("wp4jetsWeight", "wp4jetsWeight", 0.0, 1.0, wp4jets_weight);

    ttbarWeight->setConstant();
    wp3jetsWeight->setConstant();
    wp4jetsWeight->setConstant();

    RooRealVar *mu = new RooRealVar("mu", "mu", 0.0, 1.0, "");
    RooAddPdf *wwModel = new RooAddPdf("wwModel", "u*effective_ww + (1-u)*SM_WW", 
            *opsModel, *smModel, *mu);

    RooAddPdf *model = new RooAddPdf("model", "Full model", 
            RooArgList(*ttbarModel, *wp3jetsModel, *wp4jetsModel, *wwModel), 
            RooArgList(*ttbarWeight, *wp3jetsWeight, *wp4jetsWeight));

    //Generate data under the alternate hypothesis
    mu->setVal(1.0);
    RooAbsData *generatedData = model->generate(*mww, 100);

    TCanvas *canvas2 = new TCanvas("CombinedModels");
    RooPlot *frame2 = mww->frame();
    //wwModel->plotOn(frame2, RooFit::LineColor(kRed), RooFit::Name("wwModel"));
    generatedData->plotOn(frame2);
    mu->setVal(0.0);
    model->plotOn(frame2, RooFit::LineColor(kBlue), RooFit::Name("nullModel"));
    mu->setVal(1.0);
    model->plotOn(frame2, RooFit::LineColor(kRed), RooFit::Name("altModel"));
    TLegend *leg2 = new TLegend(0.65,0.73,0.86,0.87);
    //leg->AddEntry(frame2->findObject("wwModel"), "SM WW Scattering model with background",
    //        "lep");
    leg->AddEntry(frame2->findObject("nullModel"), 
            "Standard Model WW Scattering with background", "lep");
    leg->AddEntry(frame2->findObject("altModel"), 
            "Effective operator WW Scattering with background", "lep");
    frame2->Draw();
    leg2->Draw();
    canvas2->Write();
    outputFile->Close();

    
    RooArgSet poi(*mu);
    RooArgSet *nullParams = (RooArgSet*) poi.snapshot(); 

    nullParams->setRealValue("mu", 0.0); 

    RooStats::ProfileLikelihoodCalculator plc(*generatedData, *model, poi, 0.05, nullParams);


    RooStats::HypoTestResult* htr = plc.GetHypoTest();
    return htr->Significance();
}

int main(int argc, char **argv) {
    char *ttbarFileName = NULL;
    char *wp3jetsFileName = NULL;
    char *wp4jetsFileName = NULL;
    char *smwwFileName = NULL;
    char *opsFileName[10];
    int nOpsFile = 0;
    char *outputFileName = NULL;
    int c;
    while(1) {
        int option_index = 0;
        static struct option long_options[] = {
            {"ttbar", required_argument, 0, 'a'},
            {"wp3jets", required_argument, 0, 'b'},
            {"wp4jets", required_argument, 0, 'c'},
            {"smww", required_argument, 0, 'd'},
            {"opsww", required_argument, 0, 'e'},
            {"output", required_argument, 0, 'f'}
        };
        c = getopt_long(argc, argv, "abcdef:", long_options, &option_index);
        if (c==-1)
            break;
        switch(c) {
            case 'a':
                ttbarFileName = optarg;
                break;
            case 'b':
                wp3jetsFileName = optarg; 
                break;
            case 'c':
                wp4jetsFileName = optarg;
                break;
            case 'd':
                smwwFileName = optarg;
                break;
            case 'e':
                opsFileName[nOpsFile] = optarg;
                nOpsFile++;
                break;
            case 'f':
                outputFileName = optarg;
                break;
        }
    }
    double cww[4] = {4.6*pow(10, -6), 4*pow(10, -6), 5.33*pow(10, -6), 6.25*pow(10, -6)};
    double lambda[4];
    for (int i = 0; i < 4; i++) {
        lambda[i] = 1.0/sqrt(cww[i]);
    }

    TFile *outputFile = new TFile(outputFileName, "RECREATE");
    outputFile->Close();
    double significance[10];
    for (int i = 0; i < nOpsFile; i++) {
        std::cerr << "using signal file " << opsFileName[i] << "\n";
        significance[i] = RunHypoTest(smwwFileName, ttbarFileName, wp3jetsFileName, wp4jetsFileName, opsFileName[i], outputFileName);
        std::cerr << "Significance = " << significance[i] << "\n";
    }
    TGraph *graph = new TGraph(nOpsFile, lambda, significance);
    graph->SetFillColor(kRed);
    graph->SetTitle("");
    graph->GetYaxis()->SetTitle("Significance (#sigma)");
    graph->GetYaxis()->CenterTitle();
    graph->GetXaxis()->SetTitle("#Lambda (GeV)");
    graph->GetXaxis()->CenterTitle();
    TFile *outputFile2 = new TFile(outputFileName, "UPDATE");
    TCanvas *canvas = new TCanvas();
    graph->Draw("A*");
    canvas->Write();
    outputFile2->Close();
}
