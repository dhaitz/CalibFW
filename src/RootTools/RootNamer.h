
#pragma once

#include <set>

#include <iomanip>

#include <vector>
#include <sstream>

#include <boost/ptr_container/ptr_vector.hpp>

#include "RootTools/RootIncludes.h"
#include "GlobalInclude.h"

namespace CalibFW
{

// increasing number for the names of temporary root histograms
extern unsigned long g_lTempNameAppend;

class RootNamer
{
public:

	static std::string GetTempHistoName()
	{
		std::stringstream tempname;
		g_lTempNameAppend++;
		tempname << "root_temp_histo_" << g_lTempNameAppend;
		return tempname.str();
	}

	/*
	static std::string GetFolderName(PtBin * pBin)
	{
		if (pBin == NULL)
			return "NoBinning_incut/";
		else
			return pBin->id() + "_incut/";
	}

	static std::string GetHistoName(std::string algoName, std::string quantName,
			InputTypeEnum inpType, int corr = 0, PtBin * pBin = NULL,
			bool isNoCut = false, TString algoNameAppend = "Zplusjet")
	{
		stringvector tagList;

		tagList.push_back(quantName);
		tagList.push_back(algoName);
		std::stringstream fullString;

		for (stringvector::iterator it = tagList.begin(); it != tagList.end(); it++)
		{
			fullString << (*it);
			if (it != (tagList.end() - 1))
				fullString << "_";
		}
		return fullString.str();
	}
	*/
};

}

