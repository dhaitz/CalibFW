#pragma once

#include "FilterBase.h"

namespace CalibFW
{

/* Filters events by delta eta (Z, leading jet)
 * The range is definded by: |eta| in [Low, High)
 * Low and High are given by FilterJetEtaLow and FilterJetEtaHigh
 */
class DeltaEtaFilter: public ZJetFilterBase
{
public:
	virtual bool DoesEventPass(ZJetEventData const& event,
							   ZJetMetaData const& metaData, ZJetPipelineSettings const& settings)
	{
		if (!metaData.HasValidJet(settings, event) || !metaData.HasValidZ())
			//No valid objects found to apply this filter
			return false;

		double deltaeta = TMath::Abs(metaData.GetValidPrimaryJet(settings, event)->p4.Eta() - metaData.GetRefZ().p4.Eta());

		return (deltaeta >= settings.GetFilterDeltaEtaLow()) &&
			   (deltaeta < settings.GetFilterDeltaEtaHigh());
	}

	virtual std::string GetFilterId()
	{
		return "deltaeta";
	}
};

}
