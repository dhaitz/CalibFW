#pragma once

#include "FilterBase.h"

namespace Artus
{

/* Filters events by delta eta (Z, leading jet)
 * The range is definded by: |eta| in [Low, High)
 * Low and High are given by FilterJetEtaLow and FilterJetEtaHigh
 */
class DeltaEtaFilter: public ZJetFilterBase
{
public:
	virtual bool DoesEventPass(ZJetEventData const& event,
							   ZJetProduct const& product, ZJetPipelineSettings const& settings)
	{
		if (!product.HasValidJet(settings, event) || !product.HasValidZ())
			//No valid objects found to apply this filter
			return false;

		double deltaeta = TMath::Abs(product.GetValidPrimaryJet(settings, event)->p4.Eta() - product.GetRefZ().p4.Eta());

		return (deltaeta >= settings.GetFilterDeltaEtaLow()) &&
			   (deltaeta < settings.GetFilterDeltaEtaHigh());
	}

	virtual std::string GetFilterId()
	{
		return "deltaeta";
	}
};

}
