/** \file writeTxt.h
	\brief Provides a function to write Histograms in .txt files.

	Histogram (TH1D) data are written to a .txt file for further processing with
	pgfplots and other plot programs.
*/

#include <fstream>
#include <iomanip>
#include <ctime>
#include <string>
#include "TROOT.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TGraphErrors.h"

/// Write a histogram to a .txt file
/** The values of a given histogram (histo) is printed to a text file called
	\c rootfile.root_path_histo.txt with some additional information about the
	histogram.
	
	Arguments are:
	\li histo: the root-histogram (TH1D)
	\li path: the directory of this histogram within the root file
	\li rootfile: the root file in which the histogram is stored
*/
int writeTxt(TH1D* histo, std::string path=".", std::string rootfile="unknown.root", double norm=1);

/// Same function for TH1F histograms
int writeTxt(TH1F* histo, std::string path=".", std::string rootfile="unknown.root", double norm=1);



