#pragma once

#include "ZJetEventPipeline/Pipeline.h"


namespace Artus
{

/*
    Matches all jets in the event by the geometry
 */

typedef GlobalProductProducerBase<ZJetEventData, ZJetProduct, ZJetPipelineSettings>
ZJetGlobalProductProducerBase;

class JetMatcher: public ZJetGlobalProductProducerBase
{

public:
	JetMatcher(std::string input_jets1, std::string input_jets2,
			   std::string matching_name, bool flavourCorrections):
		m_jets1(input_jets1), m_jets2(input_jets2), m_matching_name(matching_name)
	{}

	JetMatcher(stringvector baseAlgos, bool flavourCorrections): m_basealgorithms(baseAlgos)
	{
		std::string prefix;
		std::vector<std::string> stringvec;
		// Check if we need to provided matched L5 jets
		if (flavourCorrections)
			stringvec = {"", "L5q", "L5b", "L5c", "L5g"};
		else
			stringvec = {""};

		// Iterate over all basealgorithms
		for (const auto & baseAlgorithm : m_basealgorithms)
		{
			// map "AK5PF" to "AK5PFJets" etc.
			if (baseAlgorithm == "AK5PF")
				prefix = "AK5PFJets";
			else if (baseAlgorithm == "AK5PFchs")
				prefix = "AK5PFJetsCHS";
			else
				LOG_FATAL(Name() << ": Couldnt process base algorithm " << baseAlgorithm)

				// Put the final algorithm in the list of algos to be matched
				for (const auto & suffix : stringvec)
				{
					m_algorithms.emplace_back(prefix + "L1L2L3" + suffix);
					m_genalgorithms.emplace_back("AK5GenJets");
				}
		}
	}

	virtual ~JetMatcher() {}

	static std::string Name()
	{
		return "jet_matcher";
	}

	virtual bool PopulateGlobalProduct(ZJetEventData const& data,
									   ZJetProduct& product,
									   ZJetPipelineSettings const& globalSettings) const;

private:
	std::string m_jets1, m_jets2, m_matching_name;
	std::vector<std::string> m_basealgorithms;
	std::vector<std::string> m_algorithms;
	std::vector<std::string> m_genalgorithms;
};

}
