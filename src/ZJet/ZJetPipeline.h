

#pragma once


#include "Filter/ZJetFilterBase.h"
#include "ZJetMetaData.h"
#include "ZJetEventData.h"

namespace CalibFW
{


class ZJetPipeline: public EventPipeline<ZJetEventData, ZJetMetaData,
		ZJetPipelineSettings>
{
public:
	// returns a list of metaproducer which are cuts and modify the cut list
	static MetaDataProducerVector GetSupportedCuts();
};



}
