#pragma once
#include "ZJetFilter/FilterBase.h"
#include "MetaData.h"
#include "EventData.h"

namespace CalibFW
{

class ZJetPipeline: public EventPipeline <ZJetEventData, ZJetMetaData, ZJetPipelineSettings>
{
public:
	// returns a list of metaproducer which are cuts and modify the cut list
	static void GetSupportedCuts(ZJetPipeline::MetaDataProducerVector& cuts);
};

}