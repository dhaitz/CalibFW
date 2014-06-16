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
		int zcount = 0;
		for (auto it = data.m_lhe->begin(); it != data.m_lhe->end(); ++it)
		{
			if (it->pdgId() == 23)
			{
				metaData.SetLHEZ(*it);
				zcount += 1;
			}
		}

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
