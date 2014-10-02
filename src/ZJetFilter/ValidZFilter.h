#pragma once

#include "FilterBase.h"

namespace Artus
{

class ValidZFilter: public ZJetFilterBase
{
public:
	virtual bool DoesEventPass(ZJetEventData const& event,
							   ZJetProduct const& product,
							   ZJetPipelineSettings const& settings)
	{
		return product.HasValidZ();
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
