#pragma once

#include "ZJetEventPipeline/Pipeline.h"

namespace Artus
{

/*
 * Sorts the Jets in the Globalb Meta Data by Transverse Momentum
 */

class JetSorter: public ZJetGlobalProductProducerBase
{
public:
	virtual void PopulateProduct(ZJetEventData const& data,
								 ZJetProduct& product,
								 ZJetPipelineSettings const& m_pipelineSettings) const
	{
		// nothing to do here
	}

	static std::string Name()
	{
		return "jet_sorter";
	}

	virtual bool PopulateGlobalProduct(ZJetEventData const& data,
									   ZJetProduct& product,
									   ZJetPipelineSettings const& globalSettings) const
	{
		product.SortJetCollections();
		return true;
	}
};


}
