#pragma once

#include <root/Math/VectorUtil.h>


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
			ZJetPipelineSettings const& m_pipelineSettings)
	{
		if ( !m_pipelineSettings.Global()->GetEnablePuReweighting())
			return;

		assert(	data.m_geneventmetadata != NULL );

		double fact = m_pipelineSettings.Global()->GetPuReweighting().at( data.m_geneventmetadata->numPUInteractions );
		metaData.SetWeight( metaData.GetWeight() * fact );
	}
};


}