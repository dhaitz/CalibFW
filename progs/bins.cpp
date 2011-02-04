#include <iostream>
#include <iomanip>
#include <vector>
#include <stdlib.h>
#include "MinimalParser.h"
#include "TFile.h"
#include "TH1D.h"
#include "TROOT.h"
#include "TString.h"

int main(int argc, char** argv)
{
	/// Hardcoded values
	const TString histoName("NoBinning_incut/z_pt_ak5PFJets_Zplusjet_data_hist");

	/// Show usage if needed.
	if (argc!=2 and argc!=3) {
		std::cout << argc<< "Usage: " << argv[0] << " file [events per bin = 50]\n";
		return 0;
	}
	int eventsPerBin = 50;
	if (argc==3) eventsPerBin = atoi(argv[2]);

	/// Read file
	TString fileName(argv[1]);
	TFile* inputfile = TFile::Open(fileName);
	if (inputfile == NULL) {
		std::cout << "File " << fileName << " not found.\n";
		return 1;
	}
	/// Read histogram
	TH1D* zpt_histo = (TH1D*) inputfile->Get(histoName);
	if (zpt_histo == NULL) {
		std::cout << "Histogram " << histoName << " not found.\n";
		return 2;
	}

	long count = 0;
	std::vector<int> counts;
	std::vector<int> bins;
	bins.push_back(zpt_histo->GetBinLowEdge(1));
	std::cout << "Calculation of optimal bin borders (≥"
			  << eventsPerBin << " events per bin):\n";
	for (int i = 1; i < zpt_histo->GetSize(); ++i)
		if (count < eventsPerBin) {
			count += zpt_histo->GetBinContent(i);
		} else {
			counts.push_back(count);
			bins.push_back(zpt_histo->GetBinLowEdge(i));
			count = zpt_histo->GetBinContent(i);
			std::cout
				<< "Bin " << std::setw(2) << bins.size()-1
				<< " from " << std::setw(3) << bins[bins.size()-2]
				<< " to " << std::setw(3) << bins.back()
				<< ":" << std::setw(5) << counts.back() << " events\n";
		}
	counts.push_back(count);
	bins.push_back(zpt_histo->GetBinLowEdge(zpt_histo->GetSize()));
	std::cout
		<< "Bin " << std::setw(2) << bins.size()-1
		<< " from" << std::setw(4) << bins[bins.size()-2]
		<< " to" << std::setw(4) << bins.back()
		<< ":" << std::setw(5) << counts.back() << " events\n";
	std::cout << "\nSummary: Bin borders:\n[" << bins.front();
//	for (std::vector<int>::const_iterator it; it=!bins.end(); ++it)
//		std::cout << ", " << bins[it];
	for (unsigned int i = 1; i < bins.size(); ++i)
		std::cout << ", " << bins[i];
	std::cout << "]\n";
}
