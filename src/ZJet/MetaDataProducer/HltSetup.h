#pragma once

#include <root/Math/VectorUtil.h>

#include "../ZJetPipeline.h"

namespace CalibFW
{

class HltSetup: public ZJetMetaDataProducerBase
{
public:
	virtual void PopulateMetaData(ZJetEventData const& data,
			ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		// nothing to do here
	}

	virtual void PopulateGlobalMetaData(ZJetEventData const& data,
			ZJetMetaData & metaData, ZJetPipelineSettings const& globalSettings) const
	{
		// optimize this and create the thing not for every event...
		//metaData.m_hlt = std::shared_ptr<HLTTools> ( new HLTTools(data.m_lumimetadata) );

	}
};


}
