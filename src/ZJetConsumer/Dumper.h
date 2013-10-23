#pragma once

#include <string>
#include <iostream>

#include <math.h>

#include "GlobalInclude.h"
#include "RootTools/RootIncludes.h"

#include "../ZJetPipeline.h"
#include "ZJetConsumer.h"

#include "../MetaDataProducer/ZJetCuts.h"
#include "../Filter/InCutFilter.h"

namespace CalibFW
{

class Dumper: public ZJetConsumerBase
{
public:
	Dumper() : ZJetConsumerBase()
	{
	}

	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData, ZJetPipelineSettings>* pset)
	{
		ZJetConsumerBase::Init(pset);
	}

	static std::string GetName()
	{
		return "dumper";
	}

	virtual void Finish()
	{
	}

	// this method is only called for events which have passed the filter imposed on the
	// pipeline
	virtual void ProcessFilteredEvent(ZJetEventData const& event, ZJetMetaData const& metaData)
	{
	}

	// this method is called for all events
	virtual void ProcessEvent(ZJetEventData const& event,
							  ZJetMetaData const& metaData,
							  FilterResult& result)
	{
		std::cout << "## Event" << std::endl << event.GetContent(this->GetPipelineSettings());
		std::cout << "## MetaData" << std::endl << metaData.GetContent();
	}

	std::map<std::string, unsigned long> m_cutRejected;
	ZJetPipeline::MetaDataProducerVector m_cuts;
	bool m_isMC;
	unsigned long m_conditional2ndJetPtCut;
	unsigned long m_conditional2ndJetPtCutBase;
	unsigned long m_conditionalDeltaPhiCut;
	unsigned long m_conditionalDeltaPhiCutBase;
	unsigned long m_eventCount;
};



}

