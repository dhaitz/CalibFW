#pragma once

#include "FlavourProducer.h"

/*
Flavour matching. Find the flavour of the leading jet according to algorithmic
or physics defintion. See
https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideBTagMCTools#Legacy_jet_flavour_definition
*/

namespace Artus
{

class FlavourProducer: public ZJetGlobalProductProducerBase
{
public:

	FlavourProducer() : ZJetGlobalProductProducerBase(), dist(0.3) {}

	virtual bool PopulateGlobalProduct(ZJetEventData const& event,
									   ZJetProduct& product, ZJetPipelineSettings const& globalsettings) const
	{

		for (const auto & jetcontainer : product.m_validPFJets)
		{
			std::string sAlgoName = jetcontainer.first;
			KGenParticles matching_algo_partons;
			KGenParticles matching_phys_partons;
			const KGenParticle* hardest_parton = NULL;
			const KGenParticle* hardest_b_quark = NULL;
			const KGenParticle* hardest_c_quark = NULL;

			// get the reference jet:genjet by default, reco jet if no genjet available
			KDataLV* ref_jet;
			std::string genName(JetType::GetGenName(sAlgoName));
			KDataLVs* genJets = SafeMap<std::string, KDataLVs*>::Get(genName, event.m_genJets);
			if (genJets->size() > 0)
				ref_jet = &genJets->at(0);
			else if (jetcontainer.second->size() > 0)
				ref_jet = & jetcontainer.second->at(0);
			else
				return 0;

			// iterate over all partons and select the ones close to the leading jet
			for (const auto & parton : product.m_genPartons)
			{
				if (std::abs(ROOT::Math::VectorUtil::DeltaR(ref_jet->p4, parton.p4)) < dist)
				{
					// Algorithmic definition: hardest b-/c-quark/parton have to be determined
					if (parton.status() != 3)
					{
						matching_algo_partons.emplace_back(parton);
						if (std::abs(parton.pdgId()) == 5 && (hardest_b_quark == NULL || parton.p4.Pt() > hardest_b_quark->p4.Pt()))
							hardest_b_quark = &parton;
						else if (std::abs(parton.pdgId()) == 4 && (hardest_c_quark == NULL || parton.p4.Pt() > hardest_c_quark->p4.Pt()))
							hardest_c_quark = &parton;
						else if (hardest_parton == NULL || parton.p4.Pt() > hardest_parton->p4.Pt())
							hardest_parton = &parton;
					}
					// Physics definition
					else
						matching_phys_partons.emplace_back(parton);
				}
			}

			// ALGORITHMIC DEFINITION
			if (matching_algo_partons.size() == 1)	  // exactly one match
				product.m_algoparton[sAlgoName] = matching_algo_partons[0];
			else if (hardest_b_quark && hardest_b_quark->p4.Pt() > 0.)
				product.m_algoparton[sAlgoName] = * hardest_b_quark;
			else if (hardest_c_quark && hardest_c_quark->p4.Pt() > 0.)
				product.m_algoparton[sAlgoName] = * hardest_c_quark;
			else if (matching_algo_partons.size() != 0)
				product.m_algoparton[sAlgoName] = * hardest_parton;

			// PHYSICS DEFINITION
			// flavour is only well defined if exactly ONE matching parton!
			if (matching_phys_partons.size() == 1)
				product.m_physparton[sAlgoName] = matching_phys_partons[0];
		}

		return true;
	}

	static std::string Name()
	{
		return "flavour_producer";
	}

	stringvector GetListOfNeededProducers()
	{
		return stringvector {GenProducer::Name()};
	}

protected:
	const float dist;

};

}
