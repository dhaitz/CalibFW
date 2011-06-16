#pragma once

#include <string>
#include <iostream>

#include <math.h>

#include "GlobalInclude.h"
#include "RootIncludes.h"
#include "EventData.h"
#include "PtBinWeighter.h"
#include "ZJetPipeline.h"
#include "ZJetConsumer.h"

#include "ZJetCuts.h"

namespace CalibFW
{

class CutStatisticsConsumer: public ZJetConsumerBase
{
public:
	CutStatisticsConsumer(ZJetCutHandler * pHandler) :
		m_pCutHandler(pHandler)
	{

	}

	virtual void Init(ZJetPipeline * pset)
	{
		m_cutRejected.clear();
		m_eventCount = 0;
		m_conditional2ndJetPtCut = 0;
		m_conditionalDeltaPhiCut = 0;

		m_conditional2ndJetPtCutBase = 0;
		m_conditionalDeltaPhiCutBase = 0;

		BOOST_FOREACH( ZJetCutBase * c, m_pCutHandler->GetCuts() )
		{
			m_cutRejected[ c->GetCutShortName() ] = 0;
		}

	ZJetConsumerBase::Init( pset );
}

virtual std::string GetId()
{
	return "cut_statistics";
}

virtual void Finish()
{
	CALIB_LOG_FILE("Cut Report for " << this->GetPipelineSettings()->GetRootFileFolder() )
	CALIB_LOG_FILE("Overall Event Count: " << m_eventCount )

	unsigned long overallCountLeft = m_eventCount;
	double droppedRel = 0.0f;

	CALIB_LOG_FILE( std::setprecision(3) << std::fixed )
	CALIB_LOG_FILE( "--- Event Cut Report ---" )
	CALIB_LOG_FILE( std::setw(20) << "CutName" << std::setw(23) << "EvtsLeftRel [%]" << std::setw(23)<< "EvtsLeft"
			<< std::setw(23)<< "EvtsDropRel [%]"<< std::setw(21) << "EvtsDropAbs")

	CALIB_LOG_FILE( std::setw(20) << "# processed events :" << std::setw(46)
			<< this->GetPipelineSettings()->GetOverallNumberOfProcessedEvents() )

	double precutsLetfRel = (double) ( overallCountLeft ) / (double) GetPipelineSettings()->GetOverallNumberOfProcessedEvents();
	CALIB_LOG_FILE( std::setw(20) << "precuts :"
			<< std::setw(23) << std::setprecision(5) << precutsLetfRel
			<< std::setw(23) << overallCountLeft
			<< std::setw(23) << std::setprecision(5) << ( 1.0f - precutsLetfRel )
			<< std::setw(21) << GetPipelineSettings()->GetOverallNumberOfProcessedEvents() - overallCountLeft)

	BOOST_FOREACH( ZJetCutBase * c, m_pCutHandler->GetCuts() )
	{
		unsigned long rejAbs = m_cutRejected[c->GetCutShortName()];


		if ( ZJetCutHandler::IsCutEnabled( c, this->GetPipelineSettings()  ) )
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

	CALIB_LOG_FILE( "-- Cut correlations --" )
	CALIB_LOG_FILE( "P( DeltaPhi | 2ndJetCut ) = "
			<< ( (float) m_conditional2ndJetPtCut/ (float ) m_conditional2ndJetPtCutBase) <<  " [ "
			<< m_conditional2ndJetPtCut << ", " << m_conditional2ndJetPtCutBase << " ]")
	CALIB_LOG_FILE( "P( 2ndJetCut | DeltaPhi ) = "
		<< ( (float) m_conditionalDeltaPhiCut/ (float ) m_conditionalDeltaPhiCutBase) <<  " [ "
		<< m_conditionalDeltaPhiCut << ", " << m_conditionalDeltaPhiCutBase << " ]")
}

// this method is only called for events which have passed the filter imposed on the
// pipeline
virtual void ProcessFilteredEvent(EventResult const& event)
{

}

// this method is called for all events
virtual void ProcessEvent(EventResult const& event, FilterResult & result)
{
	// TODO: only use events, which did pass all filters except incut

	m_eventCount++;
	BOOST_FOREACH( ZJetCutBase * c, m_pCutHandler->GetCuts() )
	{
		if (  ZJetCutHandler::IsCutEnabled( c, this->GetPipelineSettings() ))
		{
			if ( event.IsCutInBitmask( c->GetId()))
			{
				m_cutRejected[ c->GetCutShortName() ]++;
				// we only want to store the number of events, which were effectively kicked by one
				// cut here
				break;
			}
		}
	}

	if ( event.IsCutInBitmask( SecondLeadingToZPtCut::CudId ))
	{
		m_conditional2ndJetPtCutBase++;
		if ( event.IsCutInBitmask( BackToBackCut::CudId ))
			m_conditional2ndJetPtCut++;
	}

	if ( event.IsCutInBitmask( BackToBackCut::CudId ))
	{
		m_conditionalDeltaPhiCutBase++;
		if ( event.IsCutInBitmask( SecondLeadingToZPtCut::CudId ))
			m_conditionalDeltaPhiCut++;
	}

}

ZJetCutHandler * m_pCutHandler;
std::map<std::string, unsigned long> m_cutRejected;

unsigned long m_conditional2ndJetPtCut;
unsigned long m_conditional2ndJetPtCutBase;

unsigned long m_conditionalDeltaPhiCut;
unsigned long m_conditionalDeltaPhiCutBase;

unsigned long m_eventCount;
};

}

