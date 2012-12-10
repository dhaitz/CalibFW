#pragma once

#include "ZJetFilterBase.h"

namespace CalibFW
{

class ResponseFilter: public ZJetFilterBase
{
public:

	enum RespType {Balance, MPF};
	RespType m_respType;

	ResponseFilter() :
		ZJetFilterBase(), m_respType(Balance)
	{
	}

	ResponseFilter(RespType method) :
		ZJetFilterBase(), m_respType(method)
	{
	}

	virtual bool DoesEventPass(ZJetEventData const& event,
			ZJetMetaData const& metaData, ZJetPipelineSettings const& settings)
	{
		double response;

		if (!metaData.HasValidZ() || !metaData.HasValidJet(settings, event))
			return false;

		if (m_respType == Balance) {
			response = metaData.GetValidPrimaryJet(settings, event)->p4.Pt();
			response /= metaData.GetRefZ().p4.Pt();
		} else {
			response = metaData.GetMPF(metaData.GetMet(event, settings));
		}

		return (response >= settings.GetFilterResponseLow() &&
		        response < settings.GetFilterResponseHigh());
	}

	virtual std::string GetFilterId() { return "response"; }

};
}
