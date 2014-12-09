#pragma once

namespace Artus
{

/*
    This producer creates two new jet collections: L5Algo and L5Phys. These
    are needed for calculating the corrected type-I MET.

    We iterate over all jets and try to match them to a parton.
    Then, according to the flavour, we take the respective L5-corrected jets
    from the L5_gJ etc. collections and put them together to the new L5Algo/L5Phys
    collections.

    This producer requires the GenProducer (for m_genPartons) and the JetCorrector
    with the L5_gJ etc. corrections to be run before, but the JetSorter must
    be run afterwards.
*/
class L5Producer: public ZJetGlobalProductProducerBase
{
public:

	L5Producer(stringvector baseAlgos) : ZJetGlobalProductProducerBase(),
		m_basealgorithms(baseAlgos)
	{
		//We need to map the base algorithms to the final algorithms because of
		// chs vs JetsCHS ...
		for (const auto & baseAlgo : m_basealgorithms)
		{
			if (std::string::npos == baseAlgo.find("chs"))
				m_algorithms.emplace_back(baseAlgo + "JetsL1L2L3");
			else
				m_algorithms.emplace_back(baseAlgo.substr(0, 5) + "JetsCHSL1L2L3");
		}
	}

	virtual bool PopulateGlobalProduct(ZJetEventData const& data,
									   ZJetProduct& product,
									   ZJetPipelineSettings const& pipelineSettings) const
	{

		// Save algorithmic and physics flavour in an int pair
		std::pair<int, int> algo_phys;

		//loop over all algorithms we want to provide the L5 corrected MPF for
		for (const auto & algorithm : m_algorithms)
		{
			// add empty vectors so theres "something" there
			product.m_validPFJets[algorithm + "L5Algo"];
			product.m_validPFJets[algorithm + "L5Phys"];

			//for (const auto& jet: product.m_validPFJets[algorithm])
			for (unsigned int i = 0; i < product.m_validPFJets[algorithm].size(); ++i)
			{
				//Get algo and phys flavour as a std::pair
				algo_phys = GetFlavour(&product.m_validPFJets[algorithm][i], data, product);

				// add the jets to the new collection
				product.AddValidJet(product.m_validPFJets[algorithm +
									GetAlgorithmName(algo_phys.first)][i],
									algorithm + "L5Algo");
				product.AddValidJet(product.m_validPFJets[algorithm +
									GetAlgorithmName(algo_phys.second)][i],
									algorithm + "L5Phys");
			}
		}
		return true;
	}

	static std::string Name()
	{
		return "l5_producer";
	}

private:
	stringvector m_basealgorithms;
	stringvector m_algorithms;

	//For a given jet, return the (absolute) algorithmic and physics flavour
	//TODO share code with the FlavourProducer
	std::pair<int, int> GetFlavour(KDataPFTaggedJet const* ref_jet, ZJetEventData const& data,
								   ZJetProduct& product) const
	{
		int algoflavour = 0;
		int physflavour = 0;
		float dist = 0.3;
		KGenParticles matching_algo_partons;
		KGenParticles matching_phys_partons;
		const KGenParticle* hardest_parton = NULL;
		const KGenParticle* hardest_b_quark = NULL;
		const KGenParticle* hardest_c_quark = NULL;

		//iterate over all gen partons
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
			algoflavour = std::abs(matching_algo_partons[0].pdgId());
		else if (hardest_b_quark && hardest_b_quark->p4.Pt() > 0.)
			algoflavour = std::abs(hardest_b_quark->pdgId());
		else if (hardest_c_quark && hardest_c_quark->p4.Pt() > 0.)
			algoflavour = std::abs(hardest_c_quark->pdgId());
		else if (matching_algo_partons.size() != 0)
			algoflavour = std::abs(hardest_parton->pdgId());

		// PHYSICS DEFINITION
		// flavour is only well defined if exactly ONE matching parton!
		if (matching_phys_partons.size() == 1)
			physflavour = std::abs(matching_phys_partons[0].pdgId());
		return std::make_pair(algoflavour, physflavour);
	}

	// For a given flavour, return the string of the matching corrjet collection
	std::string GetAlgorithmName(int const& flavour) const
	{
		if (flavour == 21)
			return "L5g";
		else if (flavour == 5)
			return "L5b";
		else if (flavour == 4)
			return "L5c";
		else if (flavour == 3)
			return "L5s";
		else if (flavour > 0 && flavour < 3)
			return "L5q";
		else
			return "";
	}

};

}
