#pragma once

#include <vector>
#include <sstream>

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>



#include "PipelineSettings.h"
#include "FilterBase.h"

namespace CalibFW
{

template<class TData, class TMetaData, class TSettings>
class EventPipeline;

class EventMetaDataBase
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
	virtual void PopulateMetaData(TData const& data, TMetaData & metaData,
			TSettings const& m_pipelineSettings) const = 0;

	virtual void PopulateGlobalMetaData(TData const& data, TMetaData & metaData,
			TSettings const& globalSettings) const
	{
		// optional
	}

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
	virtual void ProcessFilteredEvent(TData const& event,
			TMetaData const& metaData)
	{
	}

	// this method is called for all events
	virtual void ProcessEvent(TData const& event, TMetaData const& metaData,
			FilterResult & result)
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

	TSettings const& GetPipelineSettings() const
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
			TSettings const& pset) const = 0;

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
			MetaDataProducerVector;
	typedef typename MetaDataProducerVector::iterator MetaDataVectorIterator;

	virtual void InitPipeline(TSettings pset, PipelineInitilizerBase<TData,
			TMetaData, TSettings> const& initializer)
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

	virtual std::string GetContent()
	{
		std::stringstream s;

		s << "== Pipeline Settings: " << std::endl << m_pipelineSettings.ToString() << std::endl;
		s << "== Pipeline Filter: ";

		for (FilterVectorIterator itfilter = m_filter.begin(); !(itfilter
				== m_filter.end()); itfilter++)
		{
			s << std::endl << itfilter->GetFilterId();
		}

		return s.str();
	}

	virtual void FinishPipeline()
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
	virtual void Run()
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
	virtual void RunEvent(TData const& evt, TMetaData const& globalMetaData)
	{

		// copy global meta data and use as input for the local meta producers
		TMetaData metaData = globalMetaData;

		for (MetaDataVectorIterator it = m_producer.begin(); !(it
				== m_producer.end()); it++)
		{
			(*it)->PopulateMetaData(evt, metaData, m_pipelineSettings);
		}


		FilterResult fres;

		for (FilterVectorIterator itfilter = m_filter.begin(); !(itfilter
				== m_filter.end()); itfilter++)
		{
			fres.SetFilterDecisions( itfilter->GetFilterId() ,
					itfilter->DoesEventPass(evt, metaData, m_pipelineSettings));
		}


		//std::cout << fres.ToString() << std::endl;

		// todo : we dont need to create this object here, i guess

		for (ConsumerVectorIterator itcons = m_consumer.begin(); !(itcons
				== m_consumer.end()); itcons++)
		{
			if (fres.HasPassed())
			{
				itcons->ProcessFilteredEvent(evt, metaData);
			}

			// ensure the event is valid, ( if coming from data )
			//if ( CutHandler::IsValidEvent( evt))
			itcons->ProcessEvent(evt, metaData, fres);
		}
	}

	virtual FilterBase<TData, TMetaData, TSettings> * FindFilter(
			std::string sFilterId)
	{

		for (FilterVectorIterator it = m_filter.begin(); !(it == m_filter.end()); it++)
		{
			if (it->GetFilterId() == sFilterId)
				return &(*it);
		}

		return NULL;
	}

	virtual TSettings const& GetSettings()
	{
		return m_pipelineSettings;
	}

	virtual void AddFilter(FilterForThisPipeline * pFilter)
	{
		if (FindFilter(pFilter->GetFilterId()) != NULL)
			throw std::exception();

		m_filter.push_back(pFilter);
	}

	virtual void AddConsumer(ConsumerForThisPipeline * pConsumer)
	{
		m_consumer.push_back(pConsumer);
	}

	virtual void AddMetaDataProducer(MetaDataProducerForThisPipeline * pProd)
	{
		m_producer.push_back(pProd);
	}

	const boost::ptr_vector<FilterBase<TData, TMetaData, TSettings> >& GetFilters()
	{
		return m_filter;
	}

	ConsumerVector m_consumer;
	FilterVector m_filter;
	MetaDataProducerVector m_producer;

	TSettings m_pipelineSettings;
};

}

