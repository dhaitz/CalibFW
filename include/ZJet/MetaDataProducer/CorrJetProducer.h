#pragma once

#include "../ZJetMetaData.h"
#include "ZJetMetaDataProducer.h"

using namespace CalibFW
{

class ValidJetProducer: public ZJetMetaDataProducerBase
{
public:
	virtual void PopulateGlobalMetaData(ZJetEventData const& event,
			ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{

	}
};



}
