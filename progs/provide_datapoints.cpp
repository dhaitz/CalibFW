#include <iostream>
#include <iomanip>
#include <vector>
#include <stdlib.h>
#include "TFile.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "TROOT.h"
#include "TString.h"

int main(int argc, char** argv)
{
	/// Show usage if needed.
	if (argc<4) {
		std::cout << "Usage: " << argv[0] << " datafile.root mcfile.root histonames\n " 
			<< argc-1 << " argument(s) given, min. 3 expected.\n";
		return 0;
	}
//	int eventsPerBin = 100;
//	if (argc>3) eventsPerBin = atoi(argv[3]);
//	//const TString histoName("NoBinning_incut/z_pt_ak5PFJetsL1L2L3_Zplusjet_data_hist");

//std::cout << "read";
	/// Read files
	TString dataFileName("mpfresp_ak5PFJetsL1CHS");
	dataFileName = argv[1];
	TString mcFileName(argv[2]);
	TFile* mcfile = TFile::Open(mcFileName, "READ");
	TFile* datafile = TFile::Open(dataFileName, "READ");
	TFile* outfile = new TFile("zjet_response_ratios.root", "RECREATE");
	if (mcfile == NULL) { std::cout << "File " << mcFileName << " not found.\n";return 1;}
	if (datafile == NULL) { std::cout << "File " << dataFileName << " not found.\n";return 1;}

	for (int n=3; n<argc; ++n) {
	TString histoName(argv[n]);
	/// Read histogram
	TGraphErrors* dataresp = (TGraphErrors*) datafile->Get(histoName);
	if (dataresp == NULL) {
		std::cout << "Histogram " << histoName << " not found in " << dataFileName << ".\n";
		return 2;
	}
	TGraphErrors* mcresp = (TGraphErrors*) mcfile->Get(histoName);
	if (mcresp == NULL) {
		std::cout << "Histogram " << histoName << " not found in " << mcFileName << ".\n";
		return 2;
	}
//std::cout << "do";
	TGraphErrors mpf_ratio(*dataresp);
	TString theName("ratio_");
	mpf_ratio.SetName(theName + dataresp->GetName());
	if (histoName.Contains("mpf")) {
		mpf_ratio.SetTitle("data/mc ratio with MPF;p_{T}^{Z}/GeV;data/MC ratio (MPF)");
	} else {
		mpf_ratio.SetTitle("data/mc ratio with p_{T} balance;p_{T}^{Z}/GeV;data/MC ratio (balance)");
	}
	double mittel = 0;
	for (int i=0; i<dataresp->GetN(); ++i) {
		Double_t a,b,x,y,c,z;
		dataresp->GetPoint(i,a,b);
		mcresp->GetPoint(i,x,y);
		if (i>0) mittel += b/y;
		mpf_ratio.SetPoint(i,0.5*(a+x),b/y);
		a = dataresp->GetErrorX(i);
		c = dataresp->GetErrorY(i);
		x = mcresp->GetErrorX(i);
		z = mcresp->GetErrorY(i);
		mpf_ratio.SetPointError(i,0.5*(a+x), c/y+b/y/y*z);
	}
	mittel /= dataresp->GetN()-1;
	std::cout << histoName << ": Mittel: "<< mittel << "\n";
	mpf_ratio.RemovePoint(0);
	int stop =0;
//	std::cout << "Write";
	mpf_ratio.Write();
	}
	outfile->Write();
}
