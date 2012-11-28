#pragma once

#include <boost/algorithm/string.hpp>
#include "GlobalInclude.h"

namespace CalibFW
{

class JetType
{
public:

	static std::string GetGenName(std::string algoname)
	{
		size_t pos = algoname.find("GenJets");
		pos = std::min(pos, algoname.find("PFJets"));
		std::string newname = algoname.substr(0, pos) + "GenJets";
		if (pos == std::string::npos)
			CALIB_LOG_FATAL("Can not convert algoname " << algoname << " to Gen (" << newname << " is not valid).");
		return newname;
	}

	// jet algo without any correction applied??
	static bool IsRaw(std::string algoname)
	{
		return boost::algorithm::ends_with(algoname, "PFJets") ||
			boost::algorithm::ends_with(algoname, "PFJetsCHS") ||
			boost::algorithm::ends_with(algoname, "CaloJets");
	}

	static bool IsPF(std::string algoname)
	{
		return boost::algorithm::contains(algoname, "PFJets");
	}

	static bool IsGen(std::string algoname)
	{
		return boost::algorithm::contains(algoname, "GenJets");
	}

	static bool IsCalo(std::string algoname)
	{
		return boost::algorithm::contains(algoname, "CaloJets");
	}

};

}
