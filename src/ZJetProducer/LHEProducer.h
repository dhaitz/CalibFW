#pragma once

namespace Artus
{

typedef GlobalProductProducerBase<ZJetEventData, ZJetProduct, ZJetPipelineSettings>
ZJetGlobalProductProducerBase;

class LHEProducer: public ZJetGlobalProductProducerBase
{
public:

	LHEProducer() : ZJetGlobalProductProducerBase() {}

	virtual bool PopulateGlobalProduct(ZJetEventData const& data,
									   ZJetProduct& product,
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
			else if (std::abs(lheparticle.pdgId()) == 23)
				product.SetLHEZ(lheparticle);

		}

		product.m_nLHEElectrons = nelectrons;
		product.m_nLHEMuons = nmuons;
		product.m_nLHETaus = ntaus;

		//#########################
		if (nmuons < 2)
			return false;
		//#########################

		return true;
	}

	static std::string Name()
	{
		return "lhe_producer";
	}
};


}
