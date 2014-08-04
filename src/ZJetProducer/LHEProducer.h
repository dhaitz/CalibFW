#pragma once

#include <boost/algorithm/string.hpp>
#include <string>

/*
taken from https://twiki.cern.ch/twiki/bin/view/Main/EGammaScaleFactors2012
*/

namespace Artus
{

typedef GlobalMetaDataProducerBase<ZJetEventData, ZJetMetaData, ZJetPipelineSettings>
ZJetGlobalMetaDataProducerBase;

class LHEProducer: public ZJetGlobalMetaDataProducerBase
{
public:

	LHEProducer() : ZJetGlobalMetaDataProducerBase() {}

	virtual bool PopulateGlobalMetaData(ZJetEventData const& data,
										ZJetMetaData& metaData,
										ZJetPipelineSettings const& globalSettings) const
	{
		int zcount = 0, nmuons = 0, ntaus = 0, nelectrons = 0;
		for (auto it = data.m_lhe->begin(); it != data.m_lhe->end(); ++it)
		{
			if (it->pdgId() == 23)
			{
				metaData.SetLHEZ(*it);
				zcount += 1;
			}
			else if (std::abs(it->pdgId()) == 11)
				nelectrons += 1;
			else if (std::abs(it->pdgId()) == 13)
				nmuons += 1;
			else if (std::abs(it->pdgId()) == 15)
				ntaus += 1;
		}

		metaData.m_nLHEElectrons = nelectrons;
		metaData.m_nLHEMuons = nmuons;
		metaData.m_nLHETaus = ntaus;

		// Can this be removed after checking the full dataset?
		if (zcount != 1)
			LOG_FATAL(zcount << " LHE Z bosons in the Event!?")
			return true;
	}

	static std::string Name()
	{
		return "lhe_producer";
	}
};


}
