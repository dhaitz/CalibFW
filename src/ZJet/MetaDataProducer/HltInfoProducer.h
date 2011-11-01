#pragma once

#include <root/Math/VectorUtil.h>


#include "ZJetMetaDataProducer.h"
#include "KappaTools/RootTools/HLTTools.h"

namespace CalibFW
{


// modifies the event weight depending on the number of reco vertices to fit the distribution in
// data
// can only be used on MC
class HltInfoProducer: public ZJetMetaDataProducerBase
{
public:

	virtual void PopulateMetaData(ZJetEventData const& data,
			ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		// noting todo here
	}

	virtual void PopulateGlobalMetaData(ZJetEventData const& data,
			ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		assert(	data.m_lumimetadata != NULL );

		metaData.m_hltInfo.reset( new HLTTools( data.m_lumimetadata ) );
	}
};


}
