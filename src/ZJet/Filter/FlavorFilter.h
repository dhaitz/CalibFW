#pragma once

#include "ZJetFilterBase.h"

namespace CalibFW {

/** Flavor filter

    Filter acording to parton flavor
    Possible choices for FilterFlavor:
    0 (no flavor accepted), 123 (uds quarks), 123456 (quarks),
    21 (gluon) or any other particle id
*/
class FlavorFilter: public ZJetFilterBase
{
public:
	virtual bool DoesEventPass(ZJetEventData const& event,
			ZJetMetaData const& metaData, ZJetPipelineSettings const& settings)
	{
		if (!metaData.HasBalancedParton())
			return false;
		const unsigned int flavor = std::abs(metaData.GetBalancedParton().pdgId());
		const unsigned int request = settings.GetFilterFlavor();
		if (request == 123)  // light quarks uds
			return (flavor == 1 || flavor == 2 || flavor == 3);
		if (request == 123456)  // quarks udscbt
			return (flavor == 1 || flavor == 2 || flavor == 3 ||
					flavor == 4 || flavor == 5 || flavor == 6);
		return (flavor == request);
	}

	virtual std::string GetFilterId()
	{
		return "flavor";
	}

	virtual std::string ToString(bool bVerbose = false)
	{
		return "Flavor filter";
	}

};

}
