#pragma once

#include "FilterBase.h"

namespace Artus
{

class ValidZFilter: public ZJetFilterBase
{
public:
	virtual bool DoesEventPass(ZJetEventData const& event,
							   ZJetMetaData const& metaData,
							   ZJetPipelineSettings const& settings)
	{
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
