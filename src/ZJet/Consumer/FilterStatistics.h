#pragma once

#include <string>
#include <iostream>

#include <math.h>

#include "GlobalInclude.h"

#include "../ZJetPipeline.h"
#include "ZJetConsumer.h"

#include "../MetaDataProducer/ZJetCuts.h"
#include "../Filter/InCutFilter.h"

namespace CalibFW
{

class FilterStatisticsConsumer: public ZJetConsumerBase
{
public:
	FilterStatisticsConsumer() : ZJetConsumerBase() {}

	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings>* pset)
	{
		ZJetConsumerBase::Init(pset);
		m_cutRejected.clear();
		m_eventCount = 0;
	}

	static std::string GetName()
	{
		return "filter_statistics";
	}

	virtual void Finish()
	{
		CALIB_LOG_FILE(std::endl << "--- Filter Report: "
			<< this->GetPipelineSettings().GetRootFileFolder()
			<< " Algo: " << this->GetPipelineSettings().GetJetAlgorithm() << " ---" )

		for (RejIterator_const it = m_cutRejected.begin();
				it != m_cutRejected.end(); ++it)
		{
			CALIB_LOG_FILE("Filter: " << std::setw(14) << it->first << std::setw(14)
				<< " Rejection Ratio: " << std::setw(12) << ((double) it->second/m_eventCount))
		}
	}

	// this method is only called for events which have passed the filter imposed on the
	// pipeline
	virtual void ProcessFilteredEvent(ZJetEventData const& event, ZJetMetaData const& metaData)
	{
	}

	// this method is called for all events
	virtual void ProcessEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData,
			FilterResult & result)
	{
		// only look at the event if it passed all filters, except the incut filter
		m_eventCount++;
		for (FilterResult::FilterDecisions::const_iterator it = result.GetFilterDecisions().begin();
				it != result.GetFilterDecisions().end(); ++it)
		{
			if (m_cutRejected.find(it->first) == m_cutRejected.end())
				m_cutRejected[it->first] = 0;

			if (!it->second)
				m_cutRejected[it->first]++;
		}
	}

	typedef std::map<std::string, unsigned long>::const_iterator RejIterator_const;
	std::map<std::string, unsigned long> m_cutRejected;


	unsigned long m_eventCount;
};

}

