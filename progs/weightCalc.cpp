#include <iostream>
#include <iomanip>
#include <vector>
#include <stdlib.h>
#include "TFile.h"
#include "TH1D.h"
#include "TROOT.h"
#include "TString.h"

///
/** Usage: weightCalc.exe pudist.root [MC]
    Return a vector for copy and paste in the config file.
*/

const short maxNpu = 25;

// http://cmssw.cvs.cern.ch/cgi-bin/cmssw.cgi/CMSSW/SimGeneral/MixingModule/python/mix_E7TeV_FlatDist10_2011EarlyData_inTimeOnly_cfi.py:
const double npu_probs_flat10[maxNpu] = {
	0.0698146584, 0.0698146584, 0.0698146584,
	0.0698146584, 0.0698146584, 0.0698146584, 0.0698146584,0.0698146584,
	0.0698146584, 0.0698146584, 0.0698146584 /* <-- 10*/,
	0.0630151648, 0.0526654164, 0.0402754482, 0.0292988928, 0.0194384503,
	0.0122016783, 0.0072070420, 0.0040036370, 0.0020278322, 0.0010739954,
	0.0004595759, 0.0002229748, 0.0001028162, 4.58337152809607E-05 /* <-- 24 */
};

// http://cmssw.cvs.cern.ch/cgi-bin/cmssw.cgi/CMSSW/SimGeneral/MixingModule/python/mix_E7TeV_FlatDist10_2011EarlyData_inTimeOnly_cfi.py:
const double npu_probs_spring11[maxNpu] = {
	0.0698146584, 0.0698146584, 0.0698146584,
	0.0698146584, 0.0698146584, 0.0698146584, 0.0698146584,0.0698146584,
	0.0698146584, 0.0698146584, 0.0698146584 /* <-- 10*/,
	0.0630151648, 0.0526654164, 0.0402754482, 0.0292988928, 0.0194384503,
	0.0122016783, 0.0072070420, 0.0040036370, 0.0020278322, 0.0010739954,
	0.0004595759, 0.0002229748, 0.0001028162, 4.58337152809607E-05 /* <-- 24 */
};

// http://cmssw.cvs.cern.ch/cgi-bin/cmssw.cgi/CMSSW/SimGeneral/MixingModule/python/mix_E7TeV_Summer_2011_50ns_PoissonOOT.py
const double npu_probs_summer11[maxNpu] = {
	0.0400676665, 0.040358009, 0.0807116334, 0.0924154156, 0.0924154156,
	0.0924154156, 0.0924154156, 0.0924154156, 0.0870356742, 0.0767913175,
	0.0636400516, 0.0494955563, 0.0362238310, 0.0249767088, 0.0162633216,
	0.0099919945, 0.0058339324, 0.0032326433, 0.0017151846, 0.0008505404,
	0.0004108859, 0.0001905137, 0.0000842383, 0.0000349390, 0.0000142801
};

std::vector<double> generate_weights(TH1D* data_npu_estimated, std::string mc="summer11");

int main(int argc, char** argv)
{
	/// Show usage if needed.
	if (argc!=2 and argc!=3) {
		std::cout << "Usage: " << argv[0] << " pudist.root\n";
		return 0;
	}

	/// Get MC from selection
	std::string mc("fall10");
	if (argc==3) mc = argv[2];

	/// Read file
	TString fileName(argv[1]);
	TFile* inputfile = TFile::Open(fileName);
	if (inputfile == NULL) {
		std::cout << "File " << fileName << " not found.\n";
		return 1;
	}

	/// Read histogram
	TString histoName("pileup");
	TH1D* pu_histo = (TH1D*) inputfile->Get(histoName);
	if (pu_histo == NULL) {
		std::cout << "Histogram " << histoName << " does not exist in " << fileName << ".\n";
		return 2;
	}

	/// Use the function generate_weights and write it to cout.
	std::vector<double> distribution = generate_weights(pu_histo, mc);
	std::cout << "[" << std::fixed << std::setprecision(10) << distribution[0];
	for (unsigned short npu=1; npu<maxNpu; ++npu)
		std::cout << ", " << distribution[npu];
	std::cout << "]\n";
	return 0;
}


/// Function from the twiki:
// https://twiki.cern.ch/twiki/bin/viewauth/CMS/PileupReweighting
std::vector<double> generate_weights(TH1D* data_npu_estimated, std::string mc)
{
	std::vector<double> result(maxNpu);
	double s = 0.0;
	try {
		for (unsigned short npu=0; npu<maxNpu; ++npu) {
			double npu_estimated = data_npu_estimated->GetBinContent(data_npu_estimated->GetXaxis()->FindBin(npu));
			s += npu_estimated;
			if (mc == "flat10")
				result[npu] = npu_estimated / npu_probs_flat10[npu];
			else if (mc == "spring11")
				result[npu] = npu_estimated / npu_probs_spring11[npu];
			else if (mc == "summer11")
				result[npu] = npu_estimated / npu_probs_summer11[npu];
			else
				throw 1;
		}
	} catch (int) {
		std::cout << "Monte Carlo distribution " << mc << " is not available.\n";
		exit(4);
	}

	// normalize weights such that the total sum of weights over the whole sample is 1.0, i.e., sum_i  result[i] * npu_probs[i] should be 1.0 (!)
	for (unsigned short npu=0; npu<maxNpu; ++npu)
		result[npu] /= s;

	return result;
}

