#pragma once

#include "ZJetProducer/MetadataProducer.h"

namespace Artus
{

/* Matches all jets in the event by the geomerty
 */

class JetMatcher: public ZJetGlobalMetaDataProducerBase
{
public:
	JetMatcher(std::string input_jets1, std::string input_jets2, std::string matching_name):
		m_jets1(input_jets1), m_jets2(input_jets2), m_matching_name(matching_name)
	{}

	JetMatcher(stringvector baseAlgos): m_basealgorithms(baseAlgos)
	{
		for (unsigned int i = 0; i < m_basealgorithms.size(); i++)
		{
			if (m_basealgorithms[i] == "AK5PF")
			{
				m_algorithms.emplace_back("AK5PFJetsL1L2L3");
				m_genalgorithms.emplace_back("AK5GenJets");
			}
			else if (m_basealgorithms[i] == "AK5PFchs")
			{
				m_algorithms.emplace_back("AK5PFJetsCHSL1L2L3");
				m_genalgorithms.emplace_back("AK5GenJets");
			}
			else
				LOG_FATAL(Name() << ": Couldnt process base algorithm " << m_basealgorithms[i])
			}
	}

	virtual ~JetMatcher() {}

	static std::string Name()
	{
		return "jet_matcher";
	}

	virtual bool PopulateGlobalMetaData(ZJetEventData const& data,
										ZJetMetaData& metaData,
										ZJetPipelineSettings const& globalSettings) const;

private:
	std::string m_jets1, m_jets2, m_matching_name;
	std::vector<std::string> m_basealgorithms;
	std::vector<std::string> m_algorithms;
	std::vector<std::string> m_genalgorithms;
};

}
