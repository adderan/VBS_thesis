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
    char *wpjetsFileName = NULL;
    char *smwwFileName = NULL;
    char *opsFileName = NULL;
    char *outputFileName = NULL;
    int c;
    while(1) {
        int option_index = 0;
        static struct option long_options[] = {
            {"ttbar", required_argument, 0, 'a'},
            {"wpjets", required_argument, 0, 'b'},
            {"smww", required_argument, 0, 'c'},
            {"opsww", required_argument, 0, 'd'},
            {"output", required_argument, 0, 'e'}
        };
        c = getopt_long(argc, argv, "abcd:", long_options, &option_index);
        if (c==-1)
            break;
        switch(c) {
            case 'a':
                ttbarFileName = optarg;
                break;
            case 'b':
                wpjetsFileName = optarg; 
                break;
            case 'c':
                smwwFileName = optarg;
                break;
            case 'd':
                opsFileName = optarg;
                break;
            case 'e':
                outputFileName = optarg;
                break;
        }
    }

    TFile *smwwFile = new TFile(smwwFileName);
    TFile *ttbarFile = new TFile(ttbarFileName);
    TFile *wpjetsFile = new TFile(wpjetsFileName);
    TFile *opsFile = new TFile(opsFileName);

    TFile *outputFile = new TFile(outputFileName, "RECREATE");

    //Histogram of standard model WW scattering invariant mass
    TH1F *smww = (TH1F*)smwwFile->Get(WW_MASS_HISTOGRAM_NAME);
    TH1F *ttbar = (TH1F*)ttbarFile->Get(WW_MASS_HISTOGRAM_NAME);
    TH1F *wpjets = (TH1F*)wpjetsFile->Get(WW_MASS_HISTOGRAM_NAME);

    //Histogram of ww-scattering with effective operator contributions
    TH1F *ops = (TH1F*)opsFile->Get(WW_MASS_HISTOGRAM_NAME);

    RooRealVar *mww = new RooRealVar("mww", "M_ww", 600, 2500, "GeV");

    RooDataHist smData("smData", "smData", RooArgList(*mww), smww);
    RooDataHist opsData("opsData", "opsData", RooArgList(*mww), ops);
    RooDataHist ttbarData("ttbarData", "ttbarData", RooArgList(*mww), ttbar);
    RooDataHist wpjetsData("wpjetsData", "wpjetsData", RooArgList(*mww), wpjets);


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

    RooRealVar *meanWPJets = new RooRealVar("meanWPJets", "meanWPJets", 0, 3000, "GeV");
    RooRealVar *sigmaWPJets = new RooRealVar("sigmaWPJets", "sigmaWPJets", 0, 3000, "GeV");
    RooGaussian *wpjetsModel = new RooGaussian("wpjetsModel", "wpjetsModel", 
            *mww, *meanWPJets, *sigmaWPJets);

    smModel->fitTo(smData);
    opsModel->fitTo(opsData);
    ttbarModel->fitTo(ttbarData);
    wpjetsModel->fitTo(wpjetsData);

    sigmaSM->setConstant();
    meanSM->setConstant();
    meanOps->setConstant();
    sigmaOps->setConstant();
    meanTTBar->setConstant();
    sigmaTTBar->setConstant();
    meanWPJets->setConstant();
    sigmaWPJets->setConstant();

    TCanvas *canvas = new TCanvas("Fits");
    RooPlot *frame = mww->frame();
    //smData.plotOn(frame, RooFit::LineColor(kBlack), RooFit::Name("smData"));

    smModel->plotOn(frame, RooFit::LineColor(kBlue), RooFit::Name("smModel"));
    ttbarModel->plotOn(frame, RooFit::LineColor(kRed), RooFit::Name("ttbarModel"));
    wpjetsModel->plotOn(frame, RooFit::LineColor(kYellow), RooFit::Name("wpjetsModel"));
    TLegend *leg = new TLegend(0.65,0.73,0.86,0.87);
    leg->AddEntry(frame->findObject("smModel"), "SM Model", "lep");
    leg->AddEntry(frame->findObject("ttbarModel"), "TTBar Model", "lep");
    leg->AddEntry(frame->findObject("wpjetsModel"), "WPJets Model", "lep");

    frame->Draw();
    leg->Draw();
    canvas->Write();

    RooRealVar *ttbarWeight = new RooRealVar("ttbarWeight", "ttbarWeight", 0.0, 1.0, 0.45);
    RooRealVar *wpjetsWeight = new RooRealVar("wpjetsWeight", "wpjetsWeight", 0.0, 1.0, 0.45);
    ttbarWeight->setConstant();
    wpjetsWeight->setConstant();

    RooRealVar *mu = new RooRealVar("mu", "mu", 0.0, 1.0, "");
    RooAddPdf *wwModel = new RooAddPdf("wwModel", "u*effective_ww + (1-u)*SM_WW", 
            *opsModel, *smModel, *mu);

    RooAddPdf *model = new RooAddPdf("model", "Full model", 
            RooArgList(*ttbarModel, *wpjetsModel, *wwModel), 
            RooArgList(*ttbarWeight, *wpjetsWeight));

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
