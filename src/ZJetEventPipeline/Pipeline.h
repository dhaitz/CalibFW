#pragma once
#include "ZJetFilter/FilterBase.h"
#include "MetaData.h"
#include "EventData.h"

namespace Artus
{

class ZJetPipeline: public EventPipeline <ZJetEventData, ZJetProduct, ZJetPipelineSettings>
{
public:
	// returns a list of metaproducer which are cuts and modify the cut list
	static void GetSupportedCuts(ZJetPipeline::ProducerVector& cuts);
};

}