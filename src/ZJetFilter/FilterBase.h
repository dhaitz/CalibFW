#pragma once

#include "ZJetEventPipeline/EventData.h"
#include "ZJetEventPipeline/MetaData.h"
#include "ZJetEventPipeline/PipelineSettings.h"

#include "ZJetFilter/FilterBase.h"

namespace Artus
{

typedef FilterBase<ZJetEventData, ZJetProduct, ZJetPipelineSettings> ZJetFilterBase;

}
