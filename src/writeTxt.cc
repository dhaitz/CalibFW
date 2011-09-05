/* \file writeTxt.cc
   \brief Provides a function to write Histograms in .txt files.

   Histogram (TH1D) data are written to a .txt file for further processing with
   pgfplots and other plot programs.
*/

#include "writeTxt.h"

///
/**


call: writeTxt(TH1D)

TH1D und F und TGraph nach txt


optionen:
xrange: beschänken
Spalten:
ixy xerr yerr (z zerr)(yn ynerr)
*/
int writeTxt(TH1D* histo, std::string path, std::string rootfile, double norm)
{
	/// Preparing text output
//	std::string fileName;
	TString fileName;
	fileName="dat/";
	fileName+=rootfile;
	fileName+="_";
	fileName+=path+"_"+histo->GetName()+".txt";
	fstream txtfile;
	txtfile.open(fileName, std::ios::out | std::ios::trunc);
	txtfile << "#Histogram:   " << histo->GetName();
	txtfile << "\n#Path:      " << path ;
	txtfile << "\n#From file: " << rootfile;
	txtfile << "\n#Date/time: " << time(NULL);
	txtfile << "\n#Norm:      " << norm;
	txtfile << "\n#Sum:       " << histo->GetSum();
	txtfile << "\n#Maximum:   " << histo->GetMaximum();
	txtfile << "\n#x range:   " << histo->GetXaxis()->GetXmin() <<".."<< histo->GetXaxis()->GetXmax();
	txtfile << "\n#x label:   " << histo->GetXaxis()->GetTitle();
	txtfile << "\n#y label:   " << histo->GetYaxis()->GetTitle();
	txtfile << "\n#title:     " << histo->GetTitle();
	txtfile << "\n#energy:    7";  // in TeV
	txtfile << "\n#lumi:      33"; // in pb^{-1}
	txtfile << "\n#  i    x           xmid        y           ynorm       yerr        ynormerr\n"
			<< std::fixed;
	txtfile.precision(6);
	double scalingFactor = 1.0/histo->GetSum();

	for (int i=1; i < histo->GetSize(); i++) {
		txtfile << std::setw(4)  << i;
		txtfile << std::setw(12) << histo->GetBinLowEdge(i);
		txtfile << std::setw(12) << histo->GetBinCenter(i);
		txtfile << std::setw(12) << histo->GetBinContent(i);
		txtfile << std::setw(12) << histo->GetBinContent(i)*scalingFactor;
		txtfile << std::setw(12) << histo->GetBinError(i);
		txtfile << std::setw(12) << histo->GetBinError(i)*scalingFactor;
		txtfile << std::endl;
	}
	txtfile.close();
	return 0;
}

int writeTxt(TH1F* histo, std::string path, std::string rootfile, double norm)
{
	writeTxt((TH1D*) histo, path, rootfile, norm);
	return 0;
}
