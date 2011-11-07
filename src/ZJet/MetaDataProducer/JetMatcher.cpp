#include "ZJet/MetaDataProducer/JetMatcher.h"

//#include "Kappamathis"

namespace CalibFW
{

bool JetMatcher::PopulateGlobalMetaData(ZJetEventData const& data,
		ZJetMetaData & metaData, ZJetPipelineSettings const& globalSettings) const
{
	//MatchingResult & mres = metaData.GetMatchingResults( m_matching_name );
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
