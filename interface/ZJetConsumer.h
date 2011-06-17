#pragma once

#include "EventPipeline.h"
#include "PipelineSettings.h"

#include "EventData.h"


// todo: move zJet specific pline settings here

namespace CalibFW
{
	typedef EventConsumerBase<ZJetEventData, ZJetMetaData, ZJetPipelineSettings> ZJetConsumerBase;
}
