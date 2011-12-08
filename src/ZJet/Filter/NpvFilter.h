#pragma once


#include "ZJetFilterBase.h"

namespace CalibFW
{

class NpvFilter: public ZJetFilterBase
{
public:


	virtual bool DoesEventPass(ZJetEventData const& event,
			ZJetMetaData const& metaData, ZJetPipelineSettings const& settings)
	{
		bool bPass = true;
		unsigned int npv = event.m_vertexSummary->nVertices;

		if ( npv < settings.GetFilterNpvLow() )
			bPass = false;

		if ( npv >= settings.GetFilterNpvHigh() )
			bPass = false;

		return bPass;
	}

	virtual std::string GetFilterId()
	{
		return "npv";
	}

};
}
