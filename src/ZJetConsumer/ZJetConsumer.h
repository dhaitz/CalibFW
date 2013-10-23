#pragma once

#include "EventPipeline/EventPipeline.h"
#include "EventPipeline/PipelineSettings.h"

// todo: move zJet specific pline settings here

namespace CalibFW
{

typedef EventConsumerBase<ZJetEventData, ZJetMetaData, ZJetPipelineSettings> ZJetConsumerBase;

class BulkConsumer : ZJetConsumerBase
{
public:
	BulkConsumer(stringvector const& plotProducts)
	{
		m_plotProducts = plotProducts;
	}

private:
	stringvector m_plotProducts;
};


/*
class LVBulkConsumer : BulkConsumer
{
public:
	LVBulkConsumer(stringvector const& plotProducts) : BulkConsumer(plotProducts)
	{}

	virtual void Init(ZJetConsumerBase * pset)
	{
		m_pipeline = pset;
		// initialize the needed histos
	}

};
*/

}
