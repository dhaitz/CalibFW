#pragma once

#include <root/Math/VectorUtil.h>

#include "../ZJetPipeline.h"

namespace CalibFW
{

/*
 * Sorts the Jets in the Globalb Meta Data by Transverse Momentum
 */

class JetSorter: public ZJetMetaDataProducerBase
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
		metaData.SortJetCollections();
	}
};


}
