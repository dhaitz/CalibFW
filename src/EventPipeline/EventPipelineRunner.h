#pragma once

#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_list.hpp>

#include "KappaTools/RootTools/libKRootTools.h"
#include "KappaTools/RootTools/HLTTools.h"

#include "EventPipeline/EventPipeline.h"

namespace Artus
{

template <class TEvent>
class EventProvider : public boost::noncopyable
{
public:
	virtual ~EventProvider() { }

	virtual TEvent const& GetCurrentEvent() const = 0;
	virtual bool GotoEvent(long long lEventNumber, HLTTools* hltInfo, int sampleinit) = 0;
	virtual long long GetOverallEventCount() const = 0;
};

/*
 * The EventPipelineRunner utilizes a user-provided EventProvider to load events and passes them to
 * all registered EventPipelines.
 * Furthermore, GlobalEventProducers can be registered, which can generate Pipeline-Independet meta data
 * of the event. This GlobalEventProducers are run before any pipeline is started and the generated data is passed to the
 * pipelines.
 */

template <class TPipeline, class TGlobalMetaProducer>
class EventPipelineRunner : public boost::noncopyable
{
public:
	typedef boost::ptr_list<TPipeline> Pipelines;
	typedef typename Pipelines::iterator PipelinesIterator;

	typedef boost::ptr_list<TGlobalMetaProducer> GlobalMetaProducer;
	typedef typename GlobalMetaProducer::iterator GlobalMetaProducerIterator;


	/*
	 * Add a pipeline. The object is destroy in the destructor of the EventPipelineRunner
	 */
	void AddPipeline(TPipeline* pline)
	{
		m_pipelines.push_back(pline);
	}

	/*
	 * Add a GlobalMetaProducer. The object is destroy in the destructor of the EventPipelineRunner
	 */
	void AddGlobalMetaProducer(TGlobalMetaProducer* metaProd)
	{
		m_globalMetaProducer.push_back(metaProd);
	}

	/*
	 * Add a range of pipelines. The object is destroy in the destructor of the EventPipelineRunner
	 */
	void AddPipelines(std::vector<TPipeline*> pVec)
	{
		BOOST_FOREACH(TPipeline * pline, pVec)
		{
			AddPipeline(pline);
		}
	}

	/*
	 * Run the GlobalMetaProducers and all pipelines.
	 */
	template<class TEvent, class TMetaData, class TSettings>
	long long RunPipelines(EventProvider<TEvent>& evtProvider, TSettings const& settings)
	{
		long long firstEvent = settings.Global()->GetSkipEvents();
		long long nEvents = evtProvider.GetOverallEventCount();
		if (settings.Global()->GetEventCount() >= 0)
			nEvents = firstEvent + settings.Global()->GetEventCount();
		if (firstEvent != 0 || nEvents != evtProvider.GetOverallEventCount())
			LOG(red << "Warning: Custom range of events: " << firstEvent << " to " << nEvents << reset);


		HLTTools* hltTools = new HLTTools;
		bool bEventValid = true;
		int sampleinit = -1;
		if (settings.Global()->GetEnableSampleReweighting())
			sampleinit = -2;

		LOG("Running over " << (nEvents - firstEvent) << " Events");
		for (long long i = firstEvent; i < nEvents; ++i)
		{
			// TODO refactor the evtProvider to clean up this mess with the hltTools
			if (!evtProvider.GotoEvent(i, hltTools, sampleinit))
				break;
			TMetaData metaDataGlobal;
			metaDataGlobal.m_hltInfo = hltTools;

			// create global meta data
			for (GlobalMetaProducerIterator it = m_globalMetaProducer.begin();
				 it != m_globalMetaProducer.end(); it++)
			{
				bEventValid = it->PopulateGlobalMetaData(evtProvider.GetCurrentEvent(), metaDataGlobal, settings);
				//LOG(it->GetContent())
				if (!bEventValid)
					break;
			}

			// run the pipelines, if the event is valid
			if (bEventValid)
			{
				if (unlikely(nEvents - firstEvent < 100)) // debug output
					LOG("Event "
						<< evtProvider.GetCurrentEvent().m_eventmetadata->nRun << ":"
						<< evtProvider.GetCurrentEvent().m_eventmetadata->nLumi << ":"
						<< evtProvider.GetCurrentEvent().m_eventmetadata->nEvent);
				for (PipelinesIterator it = m_pipelines.begin(); it != m_pipelines.end(); it++)
				{
					if (it->GetSettings().GetLevel() == 1)
					{
						if (unlikely(nEvents - firstEvent < 5)) // debug output
							LOG("Event:" << i
								<< ", new pipeline: " << it->GetSettings().ToString()
								<< ", algorithm: " << it->GetSettings().GetJetAlgorithm());
						it->RunEvent(evtProvider.GetCurrentEvent(), metaDataGlobal);
					}
				}
			}
			metaDataGlobal.ClearContent();
		}

		// first safe the results ( > plots ) from all level one pipelines
		for (PipelinesIterator it = m_pipelines.begin();
			 !(it == m_pipelines.end()); it++)
		{
			if (it->GetSettings().GetLevel() == 1)
				it->FinishPipeline();
		}
		/*
		// run the pipelines greater level one
		for (unsigned int i = 2; i < 10; i++)
		{
			for (PipelinesIterator it = m_pipelines.begin(); it != m_pipelines.end(); it++)
			{
				if (it->GetSettings().GetLevel() == i)
				{
					it->Run();
					it->FinishPipeline();
				}
				}
		}
		*/

		delete hltTools;
		return (nEvents - firstEvent);
	}


private:

	Pipelines m_pipelines;
	GlobalMetaProducer m_globalMetaProducer;
};

}
