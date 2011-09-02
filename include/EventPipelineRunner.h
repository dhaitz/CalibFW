#pragma once

#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_list.hpp>

#include "RootTools/libKRootTools.h"

#include "EventPipeline.h"

namespace CalibFW
{

template < class TEvent >
class EventProvider : public boost::noncopyable
{
public:
	virtual ~EventProvider()
	{
	}

	virtual TEvent const& GetCurrentEvent() const = 0;
	virtual bool GotoEvent( long long lEventNumber ) = 0;
	virtual long long GetOverallEventCount() const = 0;
};

template < class TPipeline, class TGlobalMetaProducer >
class EventPipelineRunner : public boost::noncopyable
{
public:
	typedef boost::ptr_list<TPipeline> Pipelines;
	typedef typename Pipelines::iterator PipelinesIterator;

	typedef boost::ptr_list<TGlobalMetaProducer> GlobalMetaProducer;
	typedef typename GlobalMetaProducer::iterator GlobalMetaProducerIterator;


	void AddPipeline( TPipeline * pline )
	{
		m_pipelines.push_back( pline );
	}

	// Global MetaProducer operate without the concrete settings of one
	// pipeline
	void AddGlobalMetaProducer( TGlobalMetaProducer * metaProd )
	{
		m_globalMetaProducer.push_back( metaProd );
	}

	void AddPipelines(std::vector<TPipeline*> pVec)
	{
		BOOST_FOREACH( TPipeline * pline, pVec )
			{
				AddPipeline(pline);
			}
	}

	template< class TEvent, class TMetaData, class TSettings>
	void RunPipelines( EventProvider< TEvent > & evtProvider, TSettings const& settings  )
	{
		long long nEvents = evtProvider.GetOverallEventCount();
		CALIB_LOG("Running over " << nEvents << " Events")

		//ProgressMonitor pm(nEvents);

		for ( long long lCur = 0; lCur < nEvents; lCur ++)
		{
			//if (!pm.Update()) break;
			evtProvider.GotoEvent( lCur );
			TMetaData metaDataGlobal;

			for( GlobalMetaProducerIterator it = m_globalMetaProducer.begin();
					it != m_globalMetaProducer.end(); it++)
			{
				it->PopulateGlobalMetaData(evtProvider.GetCurrentEvent(), metaDataGlobal, settings);
			}

			for( PipelinesIterator it = m_pipelines.begin(); it != m_pipelines.end(); it++)
			{
				if (it->GetSettings().GetLevel() == 1)
				{
					//CALIB_LOG( it->GetContent() )

					it->RunEvent( evtProvider.GetCurrentEvent(), metaDataGlobal );
				}

			}
		}

		// first safe the results ( > plots ) from all level one pipelines
		for (PipelinesIterator it = m_pipelines.begin();
				!(it == m_pipelines.end()); it++)
		{
			if (it->GetSettings().GetLevel() == 1)
			{
				it->FinishPipeline();
			}
		}

		// run the pipelines greater level one
		for( unsigned int i = 2; i < 10; i++)
		{

			for( PipelinesIterator it = m_pipelines.begin(); it != m_pipelines.end(); it++)
			{
				if (it->GetSettings().GetLevel() == i)
				{
					//CALIB_LOG( it->GetContent() )

					it->Run( );
					it->FinishPipeline();
				}

			}
		}


	}

private:

	Pipelines m_pipelines;
	GlobalMetaProducer m_globalMetaProducer;
};

}
