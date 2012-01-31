#include "ZJet/MetaDataProducer/JetMatcher.h"

#include <KappaTools/RootTools/Matching.h>

namespace CalibFW
{

bool JetMatcher::PopulateGlobalMetaData(ZJetEventData const& data,
		ZJetMetaData & metaData, ZJetPipelineSettings const& globalSettings) const
{
    // make this more generic
    //std::cout << std::endl << "Start Matching ";
    
    KDataPFJets & pfJets = metaData.GetPFValidJetCollection( "AK5PFJetsL1L2L3" );
    KDataLVs * genJets = data.GetGenJetCollection( "AK5GenJets" );

    std::vector<int> matching_result =
        matchSort_Matrix< KDataPFJet, KDataLV >
                        ( pfJets, pfJets.size(),
                          *genJets, genJets->size(),
                         0.3);

    metaData.m_matchingResults["AK5GenJets"] = matching_result;

    //std::cout << "done";

	return true;
}


}
