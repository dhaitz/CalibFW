#pragma once

#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_list.hpp>

#include "RootTools/libKRootTools.h"
#include "KappaTools/RootTools/HLTTools.h"

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
	virtual bool GotoEvent( long long lEventNumber, std::shared_ptr< HLTTools > & hltInfo ) = 0;
	virtual long long GetOverallEventCount() const = 0;
};

/*
 * The EventPipelineRunner utilizes a user-provided EventProvider to load events and passes them to
 * all registered EventPipelines.
 * Furthermore, GlobalEventProducers can be registered, which can generate Pipeline-Independet meta data
 * of the event. This GlobalEventProducers are run before any pipeline is started and the generated data is passed to the
 * pipelines.
 */

template < class TPipeline, class TGlobalMetaProducer >
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
	void AddPipeline( TPipeline * pline )
	{
		m_pipelines.push_back( pline );
	}

	/*
	 * Add a GlobalMetaProducer. The object is destroy in the destructor of the EventPipelineRunner
	 */
	void AddGlobalMetaProducer( TGlobalMetaProducer * metaProd )
	{
		m_globalMetaProducer.push_back( metaProd );
	}

	/*
	 * Add a range of pipelines. The object is destroy in the destructor of the EventPipelineRunner
	 */
	void AddPipelines(std::vector<TPipeline*> pVec)
	{
		BOOST_FOREACH( TPipeline * pline, pVec )
			{
				AddPipeline(pline);
			}
	}

	/*
	 * Run the GlobalMetaProducers and all pipelines.
	 */
	template< class TEvent, class TMetaData, class TSettings>
	void RunPipelines( EventProvider< TEvent > & evtProvider, TSettings const& settings  )
	{
		long long nEvents = evtProvider.GetOverallEventCount();
		CALIB_LOG("Running over " << nEvents << " Events")

		std::shared_ptr< HLTTools > hltTools( new HLTTools());

        bool bEventValid = true;

		for ( long long lCur = 0; lCur < nEvents; ++ lCur)
		{
		    // TODO refactor the evtProvider to clean up this mess with the hltTools
			evtProvider.GotoEvent( lCur , hltTools );
			TMetaData metaDataGlobal;
			metaDataGlobal.m_hltInfo = hltTools;

			// create global meta data
			for( GlobalMetaProducerIterator it = m_globalMetaProducer.begin();
					it != m_globalMetaProducer.end(); it++)
			{
				bEventValid = it->PopulateGlobalMetaData(evtProvider.GetCurrentEvent(), metaDataGlobal, settings);

				if ( !bEventValid )
				{
                    break;
				}
			}

			// run the pipelines, if the event is valid
            if ( bEventValid )
            {
                for( PipelinesIterator it = m_pipelines.begin(); it != m_pipelines.end(); it++)
                {
                    if (it->GetSettings().GetLevel() == 1)
                    {
                        //CALIB_LOG( it->GetContent() )

                        it->RunEvent( evtProvider.GetCurrentEvent(), metaDataGlobal );
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