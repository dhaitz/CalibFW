#pragma once


#include "ZJetFilterBase.h"

namespace CalibFW
{

class RunRangeFilter: public ZJetFilterBase
{
public:


	virtual bool DoesEventPass(ZJetEventData const& event,
			ZJetMetaData const& metaData, ZJetPipelineSettings const& settings)
	{
		bool bPass = true;
		unsigned int run = event.m_eventmetadata->nRun;

		if ( run < settings.GetFilterRunRangeLow() )
			bPass = false;

		if ( run >= settings.GetFilterRunRangeHigh() )
			bPass = false;

		return bPass;
	}

	virtual std::string GetFilterId()
	{
		return "runrange";
	}

};
}
