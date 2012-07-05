#pragma once

#include <vector>
#include <sstream>

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include "PipelineSettings.h"
#include "FilterBase.h"
#include "EventConsumerBase.h"
#include "MetaDataProducerBase.h"

namespace CalibFW
{

template<class TData, class TMetaData, class TSettings>
class EventPipeline;

/*

  Base class for your custom PipelineInitializer. Your custom code
  can add Filters and Consumers to newly created pipelines.
 */

template<class TData, class TMetaData, class TSettings>
class PipelineInitilizerBase
{
public:
	virtual void InitPipeline(
			EventPipeline<TData, TMetaData, TSettings> * pLine,
			TSettings const& pset) const = 0;

};


/*

 \brief Base implementation of the EventPipeline paradigm

  The EventPipline contains settings, filter and MetaDataProducer and Consumer which, when combined,
  produce the desired output of a pipline as soon as Events are send to the pipeline. An incoming event
  must not be changed by the pipeline but the pipeline can createt additional data for an event using
  MetaDataProducers.
  Most of the time, the EventPipeline will not be used stand-alone but by an EventPipelineRunner class.

  The intention of the
  different components is outlined in the following:


  - Settings
  Contain all specifics for the behaviour of this pipeline. The Settings object of type TSettings must be used
  to steer the behaviour of the MetaDataProducers, Filters and Consumers

  - MetaDataProducers
  Create additional, pipeline-specific, data for an event and stores this information in a TMetaData object

  - Filter
  Filter decide wether an input event is suitable to be processed by this pipeline. An event might not be in the desired
  PtRange and is therefore not useful for this pipline. The FilterResult is stored and Consumers can access the outcome of the
  Filter process.

  - Consumer
  The Consumer can access the input event, the created metadata, the settings and the filter result and produce the output they
  desire, like Histograms -> PLOTS PLOTS PLOTS

  Execution order is easy:
  MetaDataProducers -> Filters -> Consumers

 */

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

	typedef LocalMetaDataProducerBase<TData, TMetaData, TSettings>
			MetaDataProducerForThisPipeline;

	typedef boost::ptr_vector< MetaDataProducerForThisPipeline >
			MetaDataProducerVector;
	typedef typename MetaDataProducerVector::iterator MetaDataVectorIterator;


	/*
	 * Virtual constructor
	 */
	virtual ~EventPipeline(){};

	/*
	  Initialize the pipline using a custom PipelineInitilizer. This PipelineInitilizerBase can
	  create specific Filters and Consumers
	 */
	virtual void InitPipeline(TSettings pset, PipelineInitilizerBase<TData,
			TMetaData, TSettings> const& initializer)
	{
		m_pipelineSettings = pset;

		initializer.InitPipeline(this, pset);

		// init Filters
		for (FilterVectorIterator itfilter = m_filter.begin(); !(itfilter
				== m_filter.end()); itfilter++)
		{
			itfilter->Init(this);
		}

		// init Consumers
		for (ConsumerVectorIterator itcons = m_consumer.begin(); !(itcons
				== m_consumer.end()); itcons++)
		{
			itcons->Init(this);
		}

	}



	/*
	  Useful debug output of the Pipeline Content
	 */
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

	/*
	  Called once all events have been passed to the pipeline
	 */
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
	 * Run the pipeline without specific event input. This is most useful for
	 * Pipelines which process output from Pipelines already run.
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
	 * The globalMetaData is meta data which is equal for all pipelines and has therefore
	 * been created only once.
	 */
	virtual void RunEvent(TData const& evt, TMetaData const& globalMetaData)
	{
		// create the pipeline local data and set the pointer to the localMetaData
        TMetaData & nonconst_metaData = const_cast< TMetaData &>( globalMetaData );
        typename TMetaData::LocalMetaDataType localMetaData;
        nonconst_metaData.SetLocalMetaData ( & localMetaData );

		// run MetaDataProducers
        // Pipeline private MetaDataProducers not supported at the momemnt
		for (MetaDataVectorIterator it = m_producer.begin(); !(it
				== m_producer.end()); it++)
		{
			(it)->PopulateLocal(evt, globalMetaData, localMetaData,  m_pipelineSettings);
		}


		FilterResult fres;

		// run Filters
		for (FilterVectorIterator itfilter = m_filter.begin(); !(itfilter
				== m_filter.end()); itfilter++)
		{
			fres.SetFilterDecisions( itfilter->GetFilterId() ,
					itfilter->DoesEventPass(evt, globalMetaData, m_pipelineSettings));
		}


		// run Consumer
		for (ConsumerVectorIterator itcons = m_consumer.begin(); !(itcons
				== m_consumer.end()); itcons++)
		{
			if (fres.HasPassed())
			{
				itcons->ProcessFilteredEvent(evt, globalMetaData);
			}

			itcons->ProcessEvent(evt, globalMetaData, fres);
		}
	}

	/*
	 * Find and return a Filter by it's id in this Pipline
	 */
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

	/*
	 * Return a reference to the settings used within this pipline
	 */
	virtual TSettings const& GetSettings() const
	{
		return m_pipelineSettings;
	}

	/*
	 * Add a new Filter to this Pipeline
	 * The object will be freed in EventPipelines destructor
	 */
	virtual void AddFilter(FilterForThisPipeline * pFilter)
	{
		if (FindFilter(pFilter->GetFilterId()) != NULL)
			throw std::exception();

		m_filter.push_back(pFilter);
	}

	/*
	 * Add a new Consumer to this Pipeline
	 * The object will be freed in EventPipelines destructor
	 */
	virtual void AddConsumer(ConsumerForThisPipeline * pConsumer)
	{
		m_consumer.push_back(pConsumer);
	}

	/*
	 * Add a new MetaDataProducer to this Pipeline
	 * The object will be freed in EventPipelines destructor
	 */
	virtual void AddMetaDataProducer(MetaDataProducerForThisPipeline * pProd)
	{
		m_producer.push_back(pProd);
	}


	/*
	 * Return a list of filters is this pipline
	 */
	const boost::ptr_vector<FilterBase<TData, TMetaData, TSettings> >& GetFilters()
	{
		return m_filter;
	}

private:
	ConsumerVector m_consumer;
	FilterVector m_filter;
	MetaDataProducerVector m_producer;

	TSettings m_pipelineSettings;
};



}

