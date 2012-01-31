#pragma once

// from ROOT
#include <Math/VectorUtil.h>

#include "ZJetMetaDataProducer.h"

namespace CalibFW
{


// modifies the event weight depending on the number of reco vertices to fit the distribution in
// data
// can only be used on MC
class PuReweightingProducer: public ZJetMetaDataProducerBase
{
public:

	virtual void PopulateMetaData(ZJetEventData const& data,
			ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		// noting todo here
	}

	static std::string Name() { return "pu_reweighting_producer"; }


	virtual bool PopulateGlobalMetaData(ZJetEventData const& data,
			ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		if ( !m_pipelineSettings.Global()->GetEnablePuReweighting())
			return true;

		assert(	data.m_geneventmetadata != NULL );
		double fact = m_pipelineSettings.Global()->GetPuReweighting().at(
			int(data.m_geneventmetadata->numPUInteractionsTruth + 0.5) );
		metaData.SetWeight( metaData.GetWeight() * fact );

		return true;
	}
};


}
