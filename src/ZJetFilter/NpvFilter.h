#pragma once

#include "FilterBase.h"

namespace Artus
{

/* Filters events by the NPV they contain
 * if low is set to 0  and high to 2, events
 * with npv=1 and 2 will pass !
 */
class NpvFilter: public ZJetFilterBase
{
public:

	virtual bool DoesEventPass(ZJetEventData const& event,
							   ZJetMetaData const& metaData,
							   ZJetPipelineSettings const& settings)
	{
		bool bPass = true;
		unsigned int npv = event.m_vertexSummary->nVertices;

		if (npv < settings.GetFilterNpvLow())
			bPass = false;

		if (npv > settings.GetFilterNpvHigh())
			bPass = false;

		return bPass;
	}

	virtual std::string GetFilterId()
	{
		return "npv";
	}

};
}
