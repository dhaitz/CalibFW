#pragma once

#include <boost/algorithm/string.hpp>

#include "GlobalInclude.h"

namespace CalibFW
{

class JetType
{
public:
  
	static std::string GetGenName( std::string algoname )
	{
	  if ( boost::algorithm::starts_with( algoname, "AK5PFJets" ))
	  {
	    return "AK5GenJets";
	  }


	  if ( boost::algorithm::starts_with( algoname, "AK7PFJets" ))
	  {
	    return "AK7GenJets";
	  }
	 
	  
	  CALIB_LOG_FATAL ( "Cant convert algoname " << algoname << " to Gen " );
	}

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
