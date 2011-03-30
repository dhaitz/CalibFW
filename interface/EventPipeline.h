#pragma once

#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/foreach.hpp>

#include <boost/property_tree/ptree.hpp>

#include <memory>
#include <vector>

#include <sstream>
#include <typeinfo>

#include "GlobalInclude.h"

#include "Json_wrapper.h"

#include "RootIncludes.h"
#include "PtBinWeighter.h"
//#include "EventData.h"
#include "CompleteJetCorrector.h"

#include "PipelineSettings.h"

#include "ZJetFilter.h"
/*
 This macro implements a Setting Propery including the property tree get\put methods
 */

namespace CalibFW
{

template <class TData, class TSettings>
class EventPipeline;


template<class TData, class TSettings>
class EventConsumerBase
{
public:
	virtual void Init(EventPipeline<TData, TSettings> * pset)
	{
		m_pipeline = pset;
	}
	virtual void Finish() = 0;

	// this method is only called for events which have passed the filter imposed on the
	// pipeline
	virtual void ProcessFilteredEvent(TData & event)
	{
	}

	// this method is called for all events
	virtual void ProcessEvent(TData & event, FilterResult & result)
	{
	}

	// this method is called for seconddary pipelines
	virtual void Process()
	{
	}

	virtual std::string GetId()
	{
		return "default";
	}

	TSettings * GetPipelineSettings()
		{
		return this->m_pipeline->GetSettings();
		}

	EventPipeline<TData, TSettings> * m_pipeline;
};

template <class TData, class TSettings>
class EventPipeline
{
public:

	typedef boost::ptr_vector<EventConsumerBase<TData, TSettings> > ConsumerVector;
	typedef typename ConsumerVector::iterator ConsumerVectorIterator;

	typedef boost::ptr_vector<FilterBase<TData, TSettings> > FilterVector;
	typedef typename FilterVector::iterator FilterVectorIterator;

	void InitPipeline(TSettings * pset)
	{
		m_pipelineSettings = pset;

		// load filter from the Settings and add them
		stringvector fvec = this->GetSettings()->GetFilter();
		BOOST_FOREACH( std::string sid, fvec )
		{ // make this more beatiful :)
			if ( sid == PtWindowFilter().GetFilterId())
				m_filter.push_back( new PtWindowFilter);
			else if ( sid == InCutFilter().GetFilterId())
				m_filter.push_back( new InCutFilter);
			else if ( sid == RecoVertFilter().GetFilterId())
				m_filter.push_back( new RecoVertFilter);
			else if ( sid == JetEtaFilter().GetFilterId())
				m_filter.push_back( new JetEtaFilter);
			else if ( sid == SecondJetRatioFilter().GetFilterId())
				m_filter.push_back( new SecondJetRatioFilter);
			else
				CALIB_LOG_FATAL( "Filter " << sid << " not found." )
		}

		for (FilterVectorIterator itfilter = m_filter.begin();
				!(itfilter== m_filter.end()); itfilter++)
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

	void Run()
	{
		for (ConsumerVectorIterator itcons = m_consumer.begin(); !(itcons
				== m_consumer.end()); itcons++)
		{
			itcons->Process();
		}
	}

	void RunEvent(TData & evt)
	{

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
				itcons->ProcessFilteredEvent(evt);

			// ensure the event is valid, ( if coming from data )
			//if ( CutHandler::IsValidEvent( evt))
			itcons->ProcessEvent(evt, fres);
		}
	}

	FilterBase<TData, TSettings> * FindFilter(std::string sFilterId)
		{

		for (FilterVectorIterator it = m_filter.begin(); !(it
				== m_filter.end()); it++)
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

	const boost::ptr_vector<FilterBase<TData, TSettings> >& GetFilters()
	{
		return m_filter;
	}

	ConsumerVector m_consumer;
	FilterVector m_filter;
	CompleteJetCorrector m_corr;

	TSettings * m_pipelineSettings;
};

}


