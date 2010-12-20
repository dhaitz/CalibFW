#ifndef __CUTSTATISTICS_H__
#define __CUTSTATISTICS_H__

#include <string>
#include <iostream>

#include <math.h>

#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/scoped_ptr.hpp>

#include "CutHandler.h"
#include "GlobalInclude.h"
#include "RootIncludes.h"
#include "EventData.h"
#include "PtBinWeighter.h"
#include "EventPipeline.h"


namespace CalibFW
{

class CutStatisticsConsumer: public EventConsumerBase<EventResult>
{
public:
	virtual void Init()
	{
		m_cutRejected.clear();
		m_eventCount = 0;

		BOOST_FOREACH( EventCutBase< EventResult *> &c, g_cutHandler.GetCuts() )
		{
			m_cutRejected[ c.GetCutShortName() ] = 0;
		}
	}

	virtual void Finish()
	{
		CALIB_LOG_FILE("Cut Report:")
		CALIB_LOG_FILE("Overall Event Count: "  << m_eventCount )

		for ( std::map<std::string, unsigned long>::iterator it = m_cutRejected.begin();
				it != m_cutRejected.end();
				it++)
		{
			CALIB_LOG_FILE((*it).first  << " : " << (*it).second << " : "
					<<  std::setprecision(5) << (((float)(*it).second)/(float)m_eventCount) )
		}
	}

	// this method is only called for events which have passed the filter imposed on the
	// pipeline
	virtual void ProcessFilteredEvent(EventResult & event)
	{

	}

	// this method is called for all events
	virtual void ProcessEvent(EventResult & event, FilterResult & result)
	{
		m_eventCount++;

		BOOST_FOREACH( EventCutBase< EventResult *> &c, g_cutHandler.GetCuts() )
		{
			if ( c.m_bCutEnabled )
			{
				if ( g_cutHandler.IsCutInBitmask( c.GetId(), event.m_cutBitmask ))
				{
					m_cutRejected[ c.GetCutShortName() ]++;
				}
			}
		}
	}

	std::map<std::string, unsigned long> m_cutRejected;
	unsigned long m_eventCount;
};


}
#endif

