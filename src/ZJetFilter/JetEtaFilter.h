#pragma once

#include "FilterBase.h"

namespace Artus
{

/*
 * Filters events by the absolute value of eta_jet1
 * The range is definded by: |eta| in [Low, High)
 * Low and High are given by FilterJetEtaLow and FilterJetEtaHigh
 */
class JetEtaFilter: public ZJetFilterBase
{
public:
	virtual bool DoesEventPass(ZJetEventData const& event,
							   ZJetProduct const& product,
							   ZJetPipelineSettings const& settings)
	{
		if (!product.HasValidJet(settings, event))
			//No valid objects found to apply this filter
			return false;

		KDataLV* jet1 = product.GetValidPrimaryJet(settings, event);
		double eta = TMath::Abs(jet1->p4.Eta());
		return (eta >= settings.GetFilterJetEtaLow()) &&
			   (eta < settings.GetFilterJetEtaHigh());
	}

	virtual std::string GetFilterId()
	{
		return "jeteta";
	}

};

}
