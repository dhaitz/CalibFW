#include "ZJet/MetaDataProducer/JetMatcher.h"

#include <KappaTools/RootTools/Matching.h>

namespace CalibFW
{

bool JetMatcher::PopulateGlobalMetaData(ZJetEventData const& data,
		ZJetMetaData & metaData, ZJetPipelineSettings const& globalSettings) const
{
    KDataPFJets & jets1  = metaData.GetPFValidJetCollection( "AK5PFJetsL1" );
    KDataPFJets & jets2  = metaData.GetPFValidJetCollection( "AK5PFJetsL1L2" );


    std::vector<int> matching_result =
        matchSort_Matrix< KDataPFJet, KDataPFJet >
                        (jets1, jets1.size(),

                         jets2, jets2.size(),
                         0.3);
/*
    metaData.GetJ

    std::vector<int> matching_result =
        matchSort_Matrix< >
                        (const std::vector<T1> &base,
                         const size_t base_size,
                         const std::vector<T2> &target,
                         const size_t target_size,
                         const double dR = 0.3)
*/
//	MatchingResult & mres = metaData.GetMatchingResults( m_matching_name );
/*
	// compute jet matching here !!
	unsigned int jet1Count = metaData.GetValidJetCount( globalSettings, m_jets1 );
	unsigned int jet2Count = metaData.GetValidJetCount( globalSettings, m_jets2 );

	// oh oh oh n x m coming up ....
	for ( unsigned int j1 = 0; j1 < jet1Count; ++ j1 )
	{

	}*/
	return true;
}


}
