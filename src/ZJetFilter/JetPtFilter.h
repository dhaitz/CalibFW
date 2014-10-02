#pragma once


#include "FilterBase.h"

namespace Artus
{


class JetPtFilter: public ZJetFilterBase
{
public:

	JetPtFilter() : ZJetFilterBase() {}

	virtual bool DoesEventPass(ZJetEventData const& event,
							   ZJetProduct const& product,
							   ZJetPipelineSettings const& settings)
	{
		bool bPass = true;
		double fBinVal;

		if (!product.HasValidJet(settings, event))
			return false;

		KDataLV* pJet = product.GetValidPrimaryJet(settings, event);
		fBinVal = pJet->p4.Pt();

		if (fBinVal < settings.GetFilterJetPtLow())
			bPass = false;
		if (fBinVal >= settings.GetFilterJetPtHigh())
			bPass = false;

		return bPass;
	}

	virtual std::string GetFilterId()
	{
		return "jetpt";
	}

	virtual std::string ToString(bool bVerbose = false)
	{
		return "";
	}

};
}
