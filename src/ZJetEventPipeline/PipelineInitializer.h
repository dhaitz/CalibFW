#pragma once

#include "ZJetFilter/FilterBase.h"
#include "ZJetEventPipeline/MetaData.h"
#include "ZJetEventPipeline/EventData.h"

namespace CalibFW
{

class ZJetPipelineInitializer: public PipelineInitilizerBase<ZJetEventData,
		ZJetMetaData, ZJetPipelineSettings>
{
public:
	~ZJetPipelineInitializer() {}

	virtual void InitPipeline(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings>* pLine, ZJetPipelineSettings const& pset) const;
};

}
