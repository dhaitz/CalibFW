#pragma once


#include "FilterBase.h"

namespace Artus
{

class RunRangeFilter: public ZJetFilterBase
{
public:

	virtual bool DoesEventPass(ZJetEventData const& event,
							   ZJetProduct const& product,
							   ZJetPipelineSettings const& settings)
	{
		bool bPass = true;
		unsigned int run = event.m_eventproduct->nRun;

		if (run < settings.GetFilterRunRangeLow())
			bPass = false;
		if (run > settings.GetFilterRunRangeHigh())
			bPass = false;

		/*
		if (settings.GetFilterRunRangeLumiLow() > 0)
		{
		    // also check for the lumi section
		    if (event.m_eventproduct->nLumi < settings.GetFilterRunRangeLumiLow())
			    bPass = false;
		    if (event.m_eventproduct->nLumi > settings.GetFilterRunRangeLumiHigh())
			    bPass = false;
		}

		if (settings.GetFilterRunRangeEventLow() > 0)
		{
		    // also check for event !
		    if (event.m_eventproduct->nEvent < settings.GetFilterRunRangeEventLow())
			    bPass = false;
		    if (event.m_eventproduct->nEvent > settings.GetFilterRunRangeEventHigh())
			    bPass = false;
		}
		*/

		return bPass;
	}

	virtual std::string GetFilterId()
	{
		return "runrange";
	}

};
}
