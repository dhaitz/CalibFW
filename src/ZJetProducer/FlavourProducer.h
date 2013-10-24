#pragma once

//#include "ZJetEventPipeline/ZJetMetaData.h"

namespace CalibFW
{

class FlavourProducer: public ZJetGlobalMetaDataProducerBase
{
public:

	FlavourProducer() : ZJetGlobalMetaDataProducerBase() {}

	virtual bool PopulateGlobalMetaData(ZJetEventData const& event,
										ZJetMetaData& metaData, ZJetPipelineSettings const& globalsettings) const
	{

		const float dist = 0.3;

		for (ZJetMetaData::MetaPFJetContainer::iterator it = metaData.m_validPFJets.begin();
			 it != metaData.m_validPFJets.end(); ++ it)
		{
			std::string sAlgoName = it->first;

			KGenParticles matching_algo_partons;
			KGenParticles matching_phys_partons;
			KGenParticle hardest_parton;
			KGenParticle hardest_b_quark;
			KGenParticle hardest_c_quark;

			// get the reference jet:genjet by default, reco jet if no genjet available
			KDataLV* ref_jet;
			std::string genName(JetType::GetGenName(sAlgoName));
			KDataLVs* genJets = SafeMap<std::string, KDataLVs*>::Get(genName, event.m_genJets);
			if (genJets->size() > 0)
				ref_jet = &genJets->at(0);
			else if (it->second->size() > 0)
			{
				KDataLV* lv = & it->second->at(0);
				ref_jet = & it->second->at(0);//lv;
			}
			else
				return 0;

			// iterate over all partons and select the ones close to the leading jet
			for (auto it = metaData.m_genPartons.begin(); it != metaData.m_genPartons.end(); ++it)
			{
				// Algorithmic:
				if (it->status() != 3)
				{
					if (std::abs(ROOT::Math::VectorUtil::DeltaR(ref_jet->p4, it->p4)) < dist)
					{
						matching_algo_partons.push_back(*it);
						if (std::abs(it->pdgId()) == 5 && it->p4.Pt() > hardest_b_quark.p4.Pt())
							hardest_b_quark = *it;
						else if (std::abs(it->pdgId()) == 4 && it->p4.Pt() > hardest_c_quark.p4.Pt())
							hardest_c_quark = *it;
						else if (it->p4.Pt() > hardest_parton.p4.Pt())
							hardest_parton = *it;
					}
				}
				// Physics
				else
				{
					if (std::abs(ROOT::Math::VectorUtil::DeltaR(ref_jet->p4, it->p4)) < dist)
						matching_phys_partons.push_back(*it);
				}
			}

			// ALGORITHMIC DEFINITION
			if (matching_algo_partons.size() == 1)      // exactly one match
				metaData.m_algoparton[sAlgoName] = matching_algo_partons[0];
			else if (hardest_b_quark.p4.Pt() > 0.)
				metaData.m_algoparton[sAlgoName] = hardest_b_quark;
			else if (hardest_c_quark.p4.Pt() > 0.)
				metaData.m_algoparton[sAlgoName] = hardest_c_quark;
			else if (matching_algo_partons.size() != 0)
				metaData.m_algoparton[sAlgoName] = hardest_parton;


			// PHYSICS DEFINITION
			// flavour is only well defined if exactly ONE matching parton!
			if (matching_phys_partons.size() == 1)
				metaData.m_physparton[sAlgoName] = matching_phys_partons[0];

			//return true;
		}
	}
	static std::string Name()
	{
		return "flavour_producer";
	}
};

}
