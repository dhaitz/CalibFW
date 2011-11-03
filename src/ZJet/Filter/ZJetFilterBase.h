#pragma once


#include "Pipeline/FilterBase.h"

#include "../ZJetEventData.h"
#include "../ZJetMetaData.h"
#include "../ZJetPipelineSettings.h"

//#include "ValidZFilter.h"
//#include "InCutFilter.h"
//#include "PtWindowFilter.h"

namespace CalibFW
{

typedef FilterBase<ZJetEventData, ZJetMetaData, ZJetPipelineSettings>
		ZJetFilterBase;

}
