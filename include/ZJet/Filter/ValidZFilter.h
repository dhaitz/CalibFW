#pragma once


#include "ZJetFilterBase.h"

namespace CalibFW
{

class ValidZFilter: public ZJetFilterBase
{
public:
	virtual bool DoesEventPass(ZJetEventData const& event,
			ZJetMetaData const& metaData, ZJetPipelineSettings const& settings)
	{
		// std::cout << "val z " << metaData.HasValidZ() << std::endl;
		return metaData.HasValidZ();
	}

	virtual std::string GetFilterId()
	{
		return "valid_z";
	}

	virtual std::string ToString(bool bVerbose = false)
	{
		return "Valid Z Filter";
	}

};

}
