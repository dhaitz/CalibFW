#pragma once

#include "FilterBase.h"

namespace Artus
{

class InCutFilter: public ZJetFilterBase
{
public:

	virtual bool DoesEventPass(ZJetEventData const& event,
							   ZJetProduct const& product,
							   ZJetPipelineSettings const& settings)
	{
		//unsigned long ignoredCut = settings.GetFilterInCutIgnored();
		// no section here is allowed to set to true again, just to false ! avoids coding errors
		//return event.IsInCutWhenIgnoringCut(ignoredCut);
		// todo
		return product.IsAllCutsPassed();
	}

	virtual std::string GetFilterId()
	{
		return InCutFilter::Id();
	}

	virtual std::string ToString(bool bVerbose = false)
	{
		return "InCut";
	}

	static std::string Id()
	{
		return "incut";
	}
};

class ValidJetFilter: public ZJetFilterBase
{
public:
	virtual bool DoesEventPass(ZJetEventData const& event,
							   ZJetProduct const& product,
							   ZJetPipelineSettings const& settings)
	{
		// std::cout << "val z " << product.HasValidZ() << std::endl;
		return product.HasValidJet(settings, event);
	}

	virtual std::string GetFilterId()
	{
		return "valid_jet";
	}

	virtual std::string ToString(bool bVerbose = false)
	{
		return "Valid Jet Filter";
	}

};

}
