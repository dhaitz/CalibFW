#pragma once


#include "FilterBase.h"

namespace Artus
{


class PtWindowFilter: public ZJetFilterBase
{
public:

	enum BinWithEnum
	{
		ZPtBinning, Jet1PtBinning
	};
	BinWithEnum m_binWith;

	PtWindowFilter() :
		ZJetFilterBase(), m_binWith(ZPtBinning)
	{
	}

	PtWindowFilter(BinWithEnum binValue) :
		ZJetFilterBase(), m_binWith(binValue)
	{
	}

	virtual bool DoesEventPass(ZJetEventData const& event,
							   ZJetMetaData const& metaData,
							   ZJetPipelineSettings const& settings)
	{
		bool bPass = true;
		double fBinVal;

		if (m_binWith == ZPtBinning)
		{
			if (metaData.HasValidZ() == false)
				return false;

			fBinVal = metaData.GetRefZ().p4.Pt();
		}
		else
		{
			if (!metaData.HasValidJet(settings, event))
				return false;

			KDataLV* pJet = metaData.GetValidPrimaryJet(settings, event);
			fBinVal = pJet->p4.Pt();
		}

		if (!(fBinVal >= settings.GetFilterPtBinLow()))
			bPass = false;

		if (!(fBinVal < settings.GetFilterPtBinHigh()))
			bPass = false;

		return bPass;
	}

	virtual std::string GetFilterId()
	{
		return "ptbin";
	}

	virtual std::string ToString(bool bVerbose = false)
	{
		std::stringstream s;

		if (bVerbose)
		{
			s << "Binned with ";
			if (m_binWith == ZPtBinning)
				s << " ZPt ";
			else
				s << " Jet1Pt ";
		}
		/*
		else
		{
			s << "Pt" << std::setprecision(0)
			<< GetPipelineSettings()->GetFilterPtBinLow() << "to"
			<< std::setprecision(0)
			<< GetPipelineSettings()->GetFilterPtBinHigh();
		}
		*/
		return s.str();
	}

};
}
