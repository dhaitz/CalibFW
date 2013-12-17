#pragma once

#include "ZJetEventPipeline/Pipeline.h"

namespace Artus
{

/*
 * Sorts the Jets in the Globalb Meta Data by Transverse Momentum
 */

class JetSorter: public ZJetGlobalMetaDataProducerBase
{
public:
	virtual void PopulateMetaData(ZJetEventData const& data,
								  ZJetMetaData& metaData,
								  ZJetPipelineSettings const& m_pipelineSettings) const
	{
		// nothing to do here
	}

	static std::string Name()
	{
		return "jet_sorter";
	}

	virtual bool PopulateGlobalMetaData(ZJetEventData const& data,
										ZJetMetaData& metaData,
										ZJetPipelineSettings const& globalSettings) const
	{
		metaData.SortJetCollections();
		return true;
	}
};


}
