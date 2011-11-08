#pragma once

#include <boost/algorithm/string.hpp>

namespace CalibFW
{

class JetType
{
public:

	static bool IsPF(std::string algoname)
	{
		return boost::algorithm::starts_with( algoname, "AK5PF") ||
				boost::algorithm::starts_with( algoname, "AK7PF") ||
				boost::algorithm::starts_with( algoname, "KT4PF") ||
				boost::algorithm::starts_with( algoname, "KT6PF");
	}

	static bool IsGen(std::string algoname)
	{
		return boost::algorithm::starts_with( algoname, "AK5Gen") ||
				boost::algorithm::starts_with( algoname, "AK7Gen") ||
				boost::algorithm::starts_with( algoname, "KT4Gen") ||
				boost::algorithm::starts_with( algoname, "KT6Gen");
	}

	static bool IsCalo(std::string algoname)
	{
		return boost::algorithm::starts_with( algoname, "AK5Calo") ||
				boost::algorithm::starts_with( algoname, "AK7Calo") ||
				boost::algorithm::starts_with( algoname, "KT4Calo") ||
				boost::algorithm::starts_with( algoname, "KT6Calo");
	}

};

}
