#pragma once

#include <vector>

#include <boost/noncopyable.hpp>

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

template < class TPipeline >
class EventPipelineRunner : public boost::noncopyable
{
public:
	typedef boost::ptr_vector<TPipeline> Pipelines;
	typedef typename Pipelines::iterator PipelinesIterator;

	void AddPipeline( TPipeline * pline )
	{
		m_pipelines.push_back( pline );
	}

	void AddPipelines(std::vector<TPipeline*> pVec)
	{
		BOOST_FOREACH( TPipeline * pline, pVec )
			{
				AddPipeline(pline);
			}
	}

	template< class TEvent>
	void RunPipelines( EventProvider< TEvent > & evtProvider )
	{
		long long nEvents = evtProvider.GetOverallEventCount();
		CALIB_LOG_FILE("Running over " << nEvents << " Events")

		ProgressMonitor pm(nEvents);

		for ( long long lCur = 0; lCur < nEvents; lCur ++)
		{
			if (!pm.Update()) break;
			evtProvider.GotoEvent( lCur );

			for( PipelinesIterator it = m_pipelines.begin(); it != m_pipelines.end(); it++)
			{
				if (it->GetSettings()->GetLevel() == 1)
					it->RunEvent( evtProvider.GetCurrentEvent() );

			}
		}

		for (PipelinesIterator it = m_pipelines.begin();
				!(it == m_pipelines.end()); it++)
		{
			it->FinishPipeline();
		}
	}

private:

	Pipelines m_pipelines;
};

}
