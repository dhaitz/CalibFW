#include "ZJetProducer/JetMatcher.h"

#include <KappaTools/RootTools/Matching.h>

namespace CalibFW
{

bool JetMatcher::PopulateGlobalMetaData(ZJetEventData const& data,
		ZJetMetaData& metaData, ZJetPipelineSettings const& globalSettings) const
{
	//CALIB_LOG("Jet matcher!" << m_basealgorithms)
	// make this more generic
	for (unsigned int i = 0; i < m_basealgorithms.size(); i++)
	{
		if (m_basealgorithms[i] == "AK5PF")
		{
			KDataPFTaggedJets& pfJets = metaData.GetPFValidJetCollection("AK5PFJetsCHSL1L2L3");
			KDataLVs* genJets = data.GetGenJetCollection("AK5GenJets");

			std::vector<int> matching_result = matchSort_Matrix<KDataLV, KDataPFTaggedJet>(
					*genJets, genJets->size(), pfJets, pfJets.size(), 0.3);

			metaData.m_matchingResults["AK5GenJets"] = matching_result;
		}
		else if (m_basealgorithms[i] == "AK7PF")
		{
			KDataPFTaggedJets& pfJets = metaData.GetPFValidJetCollection("AK7PFJetsCHSL1L2L3");
			KDataLVs* genJets = data.GetGenJetCollection("AK7GenJets");

			std::vector<int> matching_result = matchSort_Matrix<KDataLV, KDataPFTaggedJet>(
					*genJets, genJets->size(), pfJets, pfJets.size(), 0.3);

			metaData.m_matchingResults["AK7GenJets"] = matching_result;
		}
	}
	return true;
}

}
