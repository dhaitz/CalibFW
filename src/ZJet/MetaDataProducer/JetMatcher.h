#pragma once

#include <root/Math/VectorUtil.h>

#include "../ZJetPipeline.h"

namespace CalibFW
{

/*
 * Matches all jets in the event by the geomerty
 */

class JetMatcher: public ZJetMetaDataProducerBase
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
		// compute jet matching here !!
	}
};


}
