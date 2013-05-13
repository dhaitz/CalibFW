#pragma once

#include "ZJetFilterBase.h"

namespace CalibFW {

/** Flavour filter

    Filter acording to parton flavour
    Possible choices for FilterFlavour:
    0 (no flavour accepted), 123 (uds quarks), 123456 (quarks),
    21 (gluon) or any other particle id
*/
class FlavourFilter: public ZJetFilterBase
{
public:
	virtual bool DoesEventPass(ZJetEventData const& event,
			ZJetMetaData const& metaData, ZJetPipelineSettings const& settings)
	{
//		other option: metaData.GetLeadingParton().pdgId())
		if (metaData.GetBalancedParton().pdgid == 0)
			return false;

		int flavour = std::abs(metaData.GetBalancedParton().pdgId());
		const int request = settings.GetFilterFlavour();
		if (unlikely(flavour > 5 && flavour != 21))
			CALIB_LOG("Strange flavour in event: " << flavour)

		if (request == 123)  // light quarks uds
			return (flavour == 1 || flavour == 2 || flavour == 3);
		if (request == 123456)  // quarks udscbt
			return (flavour == 1 || flavour == 2 || flavour == 3 ||
					flavour == 4 || flavour == 5 || flavour == 6);
		if (unlikely(request > 5 && request != 21))
			CALIB_LOG("Are you sure to request flavour " << request << "?")
		return (flavour == request);
	}

	virtual std::string GetFilterId()
	{
		return "flavour";
	}

	virtual std::string ToString(bool bVerbose = false)
	{
		return "Flavour filter";
	}

};

}
