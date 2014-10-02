#pragma once

#include "ZJetFilter/FilterBase.h"
#include "ZJetEventPipeline/MetaData.h"
#include "ZJetEventPipeline/EventData.h"

namespace Artus
{

class ZJetPipelineInitializer: public PipelineInitilizerBase<ZJetEventData, ZJetProduct, ZJetPipelineSettings>
{
public:
	~ZJetPipelineInitializer() {}

	virtual void InitPipeline(
		EventPipeline<ZJetEventData, ZJetProduct, ZJetPipelineSettings>* pLine,
		ZJetPipelineSettings const& pset) const;
};

}
