#pragma once

#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include "PipelineSettings.h"
#include "FilterBase.h"

namespace CalibFW
{

template<class TData, class TMetaData, class TSettings>
class EventPipeline;

class EventMetaDataBase: public boost::noncopyable
{
public:
	virtual ~EventMetaDataBase()
	{
	}
};

/*
 * This producer creates meta-data for a pipeline and event before the filter or the consumer are run
 * Meta data producer have to be stateless since they are used by multiple threads
 */
template<class TData, class TMetaData, class TSettings>
class MetaDataProducerBase: public boost::noncopyable
{
public:
	virtual void CreateMetaData(TData const& data, TMetaData const& metaData,
			TSettings *m_pipelineSettings) = 0;

};

template<class TData, class TMetaData, class TSettings>
class EventConsumerBase: public boost::noncopyable
{
public:
	virtual ~EventConsumerBase()
	{
	}
	virtual void Init(EventPipeline<TData, TMetaData, TSettings> * pset)
	{
		m_pipeline = pset;
	}
	virtual void Finish() = 0;

	// this method is only called for events which have passed the filter imposed on the
	// pipeline
	virtual void ProcessFilteredEvent(TData const& event, TMetaData const& metaData)
	{
	}

	// this method is called for all events
	virtual void ProcessEvent(TData const& event, TMetaData const& metaData, FilterResult & result)
	{
	}

	// this method is called for secondary pipelines
	virtual void Process()
	{
	}

	virtual std::string GetId() const
	{
		return "default";
	}

	TSettings * GetPipelineSettings() const
	{
		return this->m_pipeline->GetSettings();
	}

	EventPipeline<TData, TMetaData, TSettings> * m_pipeline;
};

template<class TData, class TMetaData, class TSettings>
class PipelineInitilizerBase
{
public:
	virtual void InitPipeline(
			EventPipeline<TData, TMetaData, TSettings> * pLine,
			TSettings * pset) = 0;

};

template<class TData, class TMetaData, class TSettings>
class EventPipeline: public boost::noncopyable
{
public:

	typedef EventConsumerBase<TData, TMetaData, TSettings>
			ConsumerForThisPipeline;
	typedef boost::ptr_vector<EventConsumerBase<TData, TMetaData, TSettings> >
			ConsumerVector;
	typedef typename ConsumerVector::iterator ConsumerVectorIterator;

	typedef FilterBase<TData, TMetaData, TSettings> FilterForThisPipeline;
	typedef boost::ptr_vector<FilterBase<TData, TMetaData, TSettings> >
			FilterVector;
	typedef typename FilterVector::iterator FilterVectorIterator;

	typedef MetaDataProducerBase<TData, TMetaData, TSettings>
			MetaDataProducerForThisPipeline;

	// this is NOT a ptr_vector, since one producer instance is used with many pipelines
	typedef std::vector<MetaDataProducerBase<TData, TMetaData, TSettings> *>
			MetaDateProducerVector;
	typedef typename MetaDateProducerVector::iterator MetaDataVectorIterator;

	void InitPipeline(TSettings * pset, PipelineInitilizerBase<TData,
			TMetaData, TSettings> & initializer)
	{
		m_pipelineSettings = pset;

		initializer.InitPipeline(this, pset);

		for (FilterVectorIterator itfilter = m_filter.begin(); !(itfilter
				== m_filter.end()); itfilter++)
		{
			itfilter->Init(this);
		}

		for (ConsumerVectorIterator itcons = m_consumer.begin(); !(itcons
				== m_consumer.end()); itcons++)
		{
			itcons->Init(this);
		}

	}

	void FinishPipeline()
	{
		for (ConsumerVectorIterator itcons = m_consumer.begin(); !(itcons
				== m_consumer.end()); itcons++)
			itcons->Finish();
		for (FilterVectorIterator itfilter = m_filter.begin(); !(itfilter
				== m_filter.end()); itfilter++)
			itfilter->Finish();

	}

	/*
	 * Run the pipeline without specific event input.
	 */
	void Run()
	{
		for (ConsumerVectorIterator itcons = m_consumer.begin(); !(itcons
				== m_consumer.end()); itcons++)
		{
			itcons->Process();
		}
	}

	/*
	 * Run the pipeline with one specific event as input
	 */
	void RunEvent(TData const& evt)
	{
		// TODO: make this faster
		TMetaData metaData;

		for (MetaDataVectorIterator it = m_producer.begin(); !(it
				== m_producer.end()); it++)
		{
			(*it)->CreateMetaData(evt, metaData, m_pipelineSettings);
		}

		bool bPassed = true;
		for (FilterVectorIterator itfilter = m_filter.begin(); !(itfilter
				== m_filter.end()); itfilter++)
		{
			if (!itfilter->DoesEventPass(evt))
			{
				bPassed = false;
				break;
			}
		}

		FilterResult fres(bPassed);

		for (ConsumerVectorIterator itcons = m_consumer.begin(); !(itcons
				== m_consumer.end()); itcons++)
		{
			if (bPassed)
				itcons->ProcessFilteredEvent(evt, metaData);

			// ensure the event is valid, ( if coming from data )
			//if ( CutHandler::IsValidEvent( evt))
			itcons->ProcessEvent(evt, metaData, fres);
		}
	}

	FilterBase<TData, TMetaData, TSettings> * FindFilter(std::string sFilterId)
	{

		for (FilterVectorIterator it = m_filter.begin(); !(it == m_filter.end()); it++)
		{
			if (it->GetFilterId() == sFilterId)
				return &(*it);
		}

		return NULL;
	}

	TSettings * GetSettings()
	{
		return m_pipelineSettings;
	}

	void AddFilter(FilterForThisPipeline * pFilter)
	{
		m_filter.push_back(pFilter);
	}

	void AddConsumer(ConsumerForThisPipeline * pConsumer)
	{
		m_consumer.push_back(pConsumer);
	}

	void AddMetaDataProducer(MetaDataProducerForThisPipeline * pProd)
	{
		m_producer.push_back(pProd);
	}

	const boost::ptr_vector<FilterBase<TData, TMetaData, TSettings> >& GetFilters()
	{
		return m_filter;
	}

	ConsumerVector m_consumer;
	FilterVector m_filter;
	MetaDateProducerVector m_producer;

	TSettings * m_pipelineSettings;
};

}

