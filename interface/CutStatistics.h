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
	CutStatisticsConsumer( CutHandler * pHandler) : m_pCutHandler ( pHandler )
	{


	}

	virtual void Init(EventPipeline * pset)
	{
		m_cutRejected.clear();
		m_eventCount = 0;

		BOOST_FOREACH( EventCutBase< EventResult *> * c, m_pCutHandler->GetCuts() )
		{
			m_cutRejected[ c->GetCutShortName() ] = 0;
		}

		EventConsumerBase<EventResult>::Init( pset );
	}

virtual std::string GetId()
{
	return "cut_statistics";
}

virtual void Finish()
{
	CALIB_LOG_FILE("Cut Report:")
	CALIB_LOG_FILE("Overall Event Count: " << m_eventCount )

	unsigned long overallCountLeft = m_eventCount;
	double droppedRel = 0.0f;

	CALIB_LOG_FILE( std::setprecision(3) << std::fixed )
	CALIB_LOG_FILE( "--- Event Cut Report ---" )
	CALIB_LOG_FILE( std::setw(20) << "CutName" << std::setw(23) << "EvtsLeftRel [%]" << std::setw(23)<< "EvtsLeft" << std::setw(23)<< "EvtsDropRel [%]"<< std::setw(21) << "EvtsDropAbs")

	CALIB_LOG_FILE( std::setw(20) << "# processed events :" << std::setw(46) << this->GetPipelineSettings()->GetOverallNumberOfProcessedEvents() )

	double precutsLetfRel = (double) ( overallCountLeft ) / (double) GetPipelineSettings()->GetOverallNumberOfProcessedEvents();
	CALIB_LOG_FILE( std::setw(20) << "precuts :"
			<< std::setw(23) << std::setprecision(5) <<  precutsLetfRel
			<< std::setw(23) << overallCountLeft
			<< std::setw(23) << std::setprecision(5) <<  ( 1.0f - precutsLetfRel )
			<< std::setw(21) << GetPipelineSettings()->GetOverallNumberOfProcessedEvents() - overallCountLeft)


	BOOST_FOREACH( EventCutBase< EventResult *> * c, m_pCutHandler->GetCuts() )
	{
		unsigned long rejAbs = m_cutRejected[c->GetCutShortName()];

		if ( c->m_bCutEnabled )
		{
			droppedRel = 1.0f -(double) ( overallCountLeft - rejAbs ) / (double) overallCountLeft;

			CALIB_LOG_FILE(std::setw(20) << c->GetCutShortName() << " : "
					<< std::setw(20) << std::setprecision(5) << (1.0f - droppedRel) * 100.0f
					<< std::setw(20) << overallCountLeft - rejAbs
					<< std::setw(20) << std::setprecision(5) << droppedRel * 100.0f
					<< std::setw(20) << rejAbs)

			overallCountLeft -= rejAbs;
		}
		else
		{
			CALIB_LOG_FILE(std::setw(20) << c->GetCutShortName() << " : disabled")
		}
	}
	CALIB_LOG_FILE( "Events left after Cuts : " << overallCountLeft )
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
	BOOST_FOREACH( EventCutBase< EventResult *>  * c, m_pCutHandler->GetCuts() )
	{
		if ( c->m_bCutEnabled )
		{
			if (  m_pCutHandler->IsCutInBitmask( c->GetId(), event.m_cutBitmask ))
			{
				m_cutRejected[ c->GetCutShortName() ]++;
				// we only want to store the number of events, which were effectively kicked by one
				// cut here
				break;
			}
		}
	}
}

CutHandler * m_pCutHandler;
std::map<std::string, unsigned long> m_cutRejected;
unsigned long m_eventCount;
};

}
#endif

