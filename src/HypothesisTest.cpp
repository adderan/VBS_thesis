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

int main(int argc, char **argv) {
    char *ttbarFileName = NULL;
    char *wp3jetsFileName = NULL;
    char *wp4jetsFileName = NULL;
    char *smwwFileName = NULL;
    char *opsFileName = NULL;
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
                opsFileName = optarg;
                break;
            case 'f':
                outputFileName = optarg;
                break;
        }
    }

    TFile *smwwFile = new TFile(smwwFileName);
    TFile *ttbarFile = new TFile(ttbarFileName);
    TFile *wp3jetsFile = new TFile(wp3jetsFileName);
    TFile *wp4jetsFile = new TFile(wp4jetsFileName);
    TFile *opsFile = new TFile(opsFileName);

    TFile *outputFile = new TFile(outputFileName, "RECREATE");
    std::cerr << "Using signal file: " << opsFileName << "\n";

    //Histogram of standard model WW scattering invariant mass
    TH1F *smww = (TH1F*)smwwFile->Get(WW_MASS_HISTOGRAM_NAME);
    TH1F *ttbar = (TH1F*)ttbarFile->Get(WW_MASS_HISTOGRAM_NAME);
    TH1F *wp3jets = (TH1F*)wp3jetsFile->Get(WW_MASS_HISTOGRAM_NAME);
    TH1F *wp4jets = (TH1F*)wp4jetsFile->Get(WW_MASS_HISTOGRAM_NAME);

    //Histogram of ww-scattering with effective operator contributions
    TH1F *ops = (TH1F*)opsFile->Get(WW_MASS_HISTOGRAM_NAME);

    RooRealVar *mww = new RooRealVar("mww", "M_ww", 600, 2500, "GeV");

    RooDataHist smData("smData", "smData", RooArgList(*mww), smww);
    RooDataHist opsData("opsData", "opsData", RooArgList(*mww), ops);
    RooDataHist ttbarData("ttbarData", "ttbarData", RooArgList(*mww), ttbar);
    RooDataHist wp3jetsData("wp3jetsData", "wp3jetsData", RooArgList(*mww), wp3jets);
    RooDataHist wp4jetsData("wp4jetsData", "wp4jetsData", RooArgList(*mww), wp4jets);


    RooRealVar *meanSM = new RooRealVar("meanSM", "meanSM", 0, 3000, "GeV");
    RooRealVar *sigmaSM = new RooRealVar("sigmaSM", "sigmaSM", 0, 3000.0, "GeV");
    RooGaussian *smModel = new RooGaussian("smModel", "smModel", *mww, *meanSM, *sigmaSM);

    RooRealVar *meanOps = new RooRealVar("meanOps", "meanOps", 0, 3000, "GeV");
    RooRealVar *sigmaOps = new RooRealVar("sigmaOps", "sigmaOps", 0, 3000, "GeV");
    RooGaussian *opsModel = new RooGaussian("opsModel", "opsModel", *mww, *meanOps, *sigmaOps);

    RooRealVar *meanTTBar = new RooRealVar("meanTTBar", "meanTTBar", 0, 3000, "GeV");
    RooRealVar *sigmaTTBar = new RooRealVar("sigmaTTBar", "sigmaTTBar", 0, 3000, "GeV");
    RooGaussian *ttbarModel = new RooGaussian("ttbarModel", "ttbarModel", *mww,
            *meanTTBar, *sigmaTTBar);

    RooRealVar *meanWP3Jets = new RooRealVar("meanWP3Jets", "meanWP3Jets", 0, 3000, "GeV");
    RooRealVar *sigmaWP3Jets = new RooRealVar("sigmaWP3Jets", "sigmaWP3Jets", 0, 3000, "GeV");
    RooGaussian *wp3jetsModel = new RooGaussian("wp3jetsModel", "wp3jetsModel", 
            *mww, *meanWP3Jets, *sigmaWP3Jets);

    RooRealVar *meanWP4Jets = new RooRealVar("meanWP4Jets", "meanWP4Jets", 0, 3000, "GeV");
    RooRealVar *sigmaWP4Jets = new RooRealVar("sigmaWP4Jets", "sigmaWP4Jets", 0, 3000, "GeV");
    RooGaussian *wp4jetsModel = new RooGaussian("wp4jetsModel", "wp4jetsModel", 
            *mww, *meanWP4Jets, *sigmaWP4Jets);

    smModel->fitTo(smData);
    opsModel->fitTo(opsData);
    ttbarModel->fitTo(ttbarData);
    wp3jetsModel->fitTo(wp3jetsData);
    wp4jetsModel->fitTo(wp4jetsData);

    sigmaSM->setConstant();
    meanSM->setConstant();
    meanOps->setConstant();
    sigmaOps->setConstant();
    meanTTBar->setConstant();
    sigmaTTBar->setConstant();
    meanWP3Jets->setConstant();
    sigmaWP3Jets->setConstant();
    meanWP4Jets->setConstant();
    sigmaWP4Jets->setConstant();

    TCanvas *canvas = new TCanvas("Fits");
    RooPlot *frame = mww->frame();
    //smData.plotOn(frame, RooFit::LineColor(kBlack), RooFit::Name("smData"));

    smModel->plotOn(frame, RooFit::LineColor(kBlue), RooFit::Name("smModel"));
    ttbarModel->plotOn(frame, RooFit::LineColor(kRed), RooFit::Name("ttbarModel"));
    wp3jetsModel->plotOn(frame, RooFit::LineColor(kYellow), RooFit::Name("wpjetsModel"));
    TLegend *leg = new TLegend(0.65,0.73,0.86,0.87);
    leg->AddEntry(frame->findObject("smModel"), "SM Model", "lep");
    leg->AddEntry(frame->findObject("ttbarModel"), "TTBar Model", "lep");
    leg->AddEntry(frame->findObject("wp3jetsModel"), "WP3Jets Model", "lep");

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
    RooAbsData *generatedData = model->generate(*mww, N_HYPOTHESIS_TEST_EVENTS);

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
    std::cout << "-------------------------------------------------" << "\n";
    std::cout << "The p-value for the null is " << htr->NullPValue() << "\n";
    std::cout << "Corresponding to a signifcance of " << htr->Significance() << "\n";
    std::cout << "-------------------------------------------------\n\n" << "\n";
}
