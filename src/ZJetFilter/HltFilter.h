#pragma once

#include <memory>

#include "KappaTools/RootTools/RunLumiReader.h"
#include "ZJetFilter/FilterBase.h"


namespace Artus
{

class HltFilter: public ZJetFilterBase
{

public:
	HltFilter() {}

	virtual bool DoesEventPass(ZJetEventData const& event,
							   ZJetProduct const& product,
							   ZJetPipelineSettings const& settings)
	{
		if (product.GetSelectedHlt().empty())
			// no HLT found
			return false;

		// TODO: Report that we changed the HLT, if we did
		// std::cout << "using trigger " << curName << std::endl;
		return event.m_eventproduct->hltFired(product.GetSelectedHlt(), event.m_lumiproduct);
	}

	virtual std::string GetFilterId()
	{
		return "hlt";
	}

	virtual std::string ToString(bool bVerbose = false)
	{
		return "Hlt Filter";
	}

private:
	std::vector<std::string> m_hltnames;
	//std::unique_ptr<RunLumiSelector> m_lumiSelector;
};

}
