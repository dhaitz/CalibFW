#pragma once


#include "Pipeline/FilterBase.h"

#include "../ZJetEventData.h"
#include "../ZJetMetaData.h"
#include "../ZJetPipelineSettings.h"

namespace CalibFW
{

typedef FilterBase<ZJetEventData, ZJetMetaData , ZJetPipelineSettings>
		ZJetFilterBase;

}
