#pragma once

#include "FilterBase.h"

namespace Artus
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
							   ZJetProduct const& product,
							   ZJetPipelineSettings const& settings)
	{
		double response;

		if (!product.HasValidZ() || !product.HasValidJet(settings, event))
			return false;

		if (m_respType == Balance)
		{
			response = product.GetValidPrimaryJet(settings, event)->p4.Pt();
			response /= product.GetRefZ().p4.Pt();
		}
		else
		{
			response = product.GetMPF(product.GetMet(event, settings));
		}

		return (response >= settings.GetFilterResponseLow() &&
				response < settings.GetFilterResponseHigh());
	}

	virtual std::string GetFilterId()
	{
		return "response";
	}

};
}
