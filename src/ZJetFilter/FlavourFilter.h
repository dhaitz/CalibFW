#pragma once

#include "FilterBase.h"

namespace Artus
{

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
							   ZJetMetaData const& metaData,
							   ZJetPipelineSettings const& settings)
	{
		// other option: metaData.GetLeadingParton().pdgId())
		try
		{
			if (!metaData.GetRefValidParton())
				return false;

			if (metaData.GetPtLeadingParton() == NULL)
				return false;

			int flavour = std::abs(metaData.GetRefLeadingParton().pdgId());
			const int request = settings.GetFilterFlavour();
			if (unlikely(flavour > 5 && flavour != 21))
				return false; //LOG_FATAL("Strange flavour in event: " << flavour);

			if (request == 123)  // light quarks uds
				return (flavour == 1 || flavour == 2 || flavour == 3);
			if (request == 123456)  // quarks udscbt
				return (flavour == 1 || flavour == 2 || flavour == 3 ||
						flavour == 4 || flavour == 5 || flavour == 6);
			if (unlikely(request > 5 && request != 21))
				LOG("Are you sure to request flavour " << request << "?")
				return (flavour == request);
		}
		catch (...)
		{
			LOG("There was a flavour error (UNXPCT)");
			return false;
		};
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
