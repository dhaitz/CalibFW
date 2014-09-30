#pragma once

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
		int nmuons = 0, ntaus = 0, nelectrons = 0;

		for (const auto & lheparticle : *data.m_lhe)
		{
			if (std::abs(lheparticle.pdgId()) == 11)
				nelectrons += 1;
			else if (std::abs(lheparticle.pdgId()) == 13)
				nmuons += 1;
			else if (std::abs(lheparticle.pdgId()) == 15)
				ntaus += 1;
		}

		metaData.m_nLHEElectrons = nelectrons;
		metaData.m_nLHEMuons = nmuons;
		metaData.m_nLHETaus = ntaus;

		return true;
	}

	static std::string Name()
	{
		return "lhe_producer";
	}
};


}
