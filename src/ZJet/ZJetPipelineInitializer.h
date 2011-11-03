

#pragma once


#include "Filter/ZJetFilterBase.h"
#include "ZJetMetaData.h"
#include "ZJetEventData.h"

namespace CalibFW
{


class ZJetPipelineInitializer: public PipelineInitilizerBase<ZJetEventData,
		ZJetMetaData, ZJetPipelineSettings>
{
public:
	virtual void
			InitPipeline(EventPipeline<ZJetEventData, ZJetMetaData,
					ZJetPipelineSettings> * pLine,
					ZJetPipelineSettings const& pset) const;
};

}
