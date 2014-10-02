#include "JetMatcher.h"

#include <KappaTools/RootTools/Matching.h>

namespace Artus
{

bool JetMatcher::PopulateGlobalProduct(ZJetEventData const& data,
									   ZJetProduct& product, ZJetPipelineSettings const& globalSettings) const
{

	for (unsigned int i = 0; i < m_algorithms.size(); i++)
	{
		KDataPFTaggedJets& pfJets = product.GetPFValidJetCollection(m_algorithms[i]);
		KDataLVs* genJets = data.GetGenJetCollection(m_genalgorithms[i]);

		std::vector<int> matching_result = matchSort_Matrix<KDataLV, KDataPFTaggedJet>(
											   *genJets, genJets->size(), pfJets, pfJets.size(), 0.3);
		product.m_matchingResults[m_algorithms[i]] = matching_result;
	}

	return true;
}

}
