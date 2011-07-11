#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include "TFile.h"
#include "TH1D.h"

/// weightCalc calculates the weights for the pile-up reweighting
/** Usage: weightCalc.exe pudist.root [MC-weights-name]
    Return a vector for copy and paste in the config file.
*/

const short maxNpu = 25;

// http://cmssw.cvs.cern.ch/cgi-bin/cmssw.cgi/CMSSW/SimGeneral/MixingModule/python/mix_E7TeV_FlatDist10_2011EarlyData_inTimeOnly_cfi.py:
// http://cmssw.cvs.cern.ch/cgi-bin/cmssw.cgi/CMSSW/SimGeneral/MixingModule/python/mix_E7TeV_FlatDist10_2011EarlyData_50ns_cfi.py
// is used for Summer11, too:
const double npu_probs_flat10[maxNpu] = {
	0.0698146584, 0.0698146584, 0.0698146584, 0.0698146584,
	0.0698146584, 0.0698146584, 0.0698146584, 0.0698146584,
	0.0698146584, 0.0698146584, 0.0698146584 /* <-- 10*/,
	0.0630151648, 0.0526654164, 0.0402754482, 0.0292988928, 0.0194384503,
	0.0122016783, 0.0072070420, 0.0040036370, 0.0020278322, 0.0010739954,
	0.0004595759, 0.0002229748, 0.0001028162, 4.58337152809607E-05 /* <-- 24 */
};

// http://cmssw.cvs.cern.ch/cgi-bin/cmssw.cgi/CMSSW/SimGeneral/MixingModule/python/mix_E7TeV_FlatDist10_2011EarlyData_inTimeOnly_cfi.py:
const double npu_probs_spring11[maxNpu] = {
	0.0698146584, 0.0698146584, 0.0698146584, 0.0698146584,
	0.0698146584, 0.0698146584, 0.0698146584, 0.0698146584,
	0.0698146584, 0.0698146584, 0.0698146584 /* <-- 10*/,
	0.0630151648, 0.0526654164, 0.0402754482, 0.0292988928, 0.0194384503,
	0.0122016783, 0.0072070420, 0.0040036370, 0.0020278322, 0.0010739954,
	0.0004595759, 0.0002229748, 0.0001028162, 4.58337152809607E-05 /* <-- 24 */
};

// http://cmssw.cvs.cern.ch/cgi-bin/cmssw.cgi/CMSSW/SimGeneral/MixingModule/python/mix_E7TeV_Summer_2011_50ns_PoissonOOT.py
const double npu_probs_summer11oot[maxNpu] = {
	0.0400676665, 0.0403580090, 0.0807116334, 0.0924154156, 0.0924154156,
	0.0924154156, 0.0924154156, 0.0924154156, 0.0870356742, 0.0767913175,
	0.0636400516, 0.0494955563, 0.0362238310, 0.0249767088, 0.0162633216,
	0.0099919945, 0.0058339324, 0.0032326433, 0.0017151846, 0.0008505404,
	0.0004108859, 0.0001905137, 0.0000842383, 0.0000349390, 0.0000142801
};

// real distribution in Summer11 MC dataset:
const double npu_probs_summer11real[maxNpu] = {
	0.0719617, 0.0722807, 0.0650920, 0.0723523, 0.0649074,
	0.0759789, 0.0800069, 0.0722771, 0.0671723, 0.0708293,
	0.0661459, 0.0581419, 0.0495951, 0.0363465, 0.0306755,
	0.0180953, 0.0121036, 0.00800573, 0.00282745, 0.00283283,
	0.0017685, 0.000198889, 0.0001, 0.000204264, 0.0001
};

std::vector<double> generate_weights(TH1D* data_npu_estimated, std::string mc="summer11real");

int main(int argc, char** argv)
{
	/// Show usage if needed.
	if (argc!=2 and argc!=3) {
		std::cout << "Usage: " << argv[0] << " pudist.root [flat10|spring11|summer11oot|summer11real]\n";
		return 0;
	}

	/// Get MC from selection
	std::string mc("flat10");
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

	/// Use the function generate_weights and write them to cout.
	std::vector<double> distribution = generate_weights(pu_histo, mc);
	std::cout << "conf[\"PUWeights\"] = [" << std::fixed << std::setprecision(10) << distribution[0];
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
			else if (mc == "summer11oot")
				result[npu] = npu_estimated / npu_probs_summer11oot[npu];
			else if (mc == "summer11real")
				result[npu] = npu_estimated / npu_probs_summer11real[npu];
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

