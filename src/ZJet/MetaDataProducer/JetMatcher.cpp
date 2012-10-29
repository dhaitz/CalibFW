#include "ZJet/MetaDataProducer/JetMatcher.h"

#include <KappaTools/RootTools/Matching.h>

namespace CalibFW
{

bool JetMatcher::PopulateGlobalMetaData(ZJetEventData const& data,
		ZJetMetaData& metaData, ZJetPipelineSettings const& globalSettings) const
{
	//CALIB_LOG("Jet matcher!" << m_basealgorithms)
	// make this more generic
	for (int i = 0; i < m_basealgorithms.size(); i++)
	{
		if (m_basealgorithms[i] == "AK5PF")
		{
			KDataPFJets& pfJets = metaData.GetPFValidJetCollection("AK5PFJetsL1L2L3");
			KDataLVs* genJets = data.GetGenJetCollection("AK5GenJets");

			std::vector<int> matching_result = matchSort_Matrix<KDataPFJet, KDataLV>
					(pfJets, pfJets.size(), *genJets, genJets->size(), 0.3);

			metaData.m_matchingResults["AK5GenJets"] = matching_result;
		}
		else if (m_basealgorithms[i] == "AK7PF")
		{
			KDataPFJets& pfJets = metaData.GetPFValidJetCollection("AK7PFJetsL1L2L3");
			KDataLVs* genJets = data.GetGenJetCollection("AK7GenJets");

			std::vector<int> matching_result = matchSort_Matrix<KDataPFJet, KDataLV>
					(pfJets, pfJets.size(), *genJets, genJets->size(), 0.3);

			metaData.m_matchingResults["AK7GenJets"] = matching_result;
		}
	}
	return true;
}

}
