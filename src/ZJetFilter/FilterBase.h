#pragma once


#include "ZJetFilter/FilterBase.h"

#include "ZJetEventPipeline/EventData.h"
#include "ZJetEventPipeline/MetaData.h"
#include "ZJetEventPipeline/PipelineSettings.h"

namespace CalibFW
{

typedef FilterBase<ZJetEventData, ZJetMetaData, ZJetPipelineSettings>
		ZJetFilterBase;

}
