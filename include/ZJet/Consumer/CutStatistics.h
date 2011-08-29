#pragma once

#include <string>
#include <iostream>

#include <math.h>

#include "GlobalInclude.h"
#include "RootIncludes.h"
#include "EventData.h"

#include "../ZJetPipeline.h"
#include "ZJetConsumer.h"

#include "../MetaDataProducer/ZJetCuts.h"
#include "../Filter/InCutFilter.h"

namespace CalibFW
{

class CutStatisticsConsumer: public ZJetConsumerBase
{
public:
	CutStatisticsConsumer() :
		ZJetConsumerBase()
		{

		}

	virtual void Init(EventPipeline<ZJetEventData, ZJetMetaData,
			ZJetPipelineSettings> * pset)
	{
		ZJetConsumerBase::Init(pset);

		m_cutRejected.clear();
		m_eventCount = 0;
		m_conditional2ndJetPtCut = 0;
		m_conditionalDeltaPhiCut = 0;

		m_conditional2ndJetPtCutBase = 0;
		m_conditionalDeltaPhiCutBase = 0;

		m_cuts = ZJetPipeline::GetSupportedCuts();

		BOOST_FOREACH( ZJetPipeline::MetaDataProducerForThisPipeline * m, m_cuts )
		{
			ZJetCutBase * c = static_cast<ZJetCutBase *>( m );
			m_cutRejected[ c->GetCutShortName() ] = 0;
		}
	}

	static std::string GetName()
	{
		return "cut_statistics";
	}

	virtual void Finish()
	{
		CALIB_LOG_FILE("Cut Report for " << this->GetPipelineSettings().GetRootFileFolder() )
		CALIB_LOG_FILE("Overall Event Count: " << m_eventCount)

		unsigned long overallCountLeft = m_eventCount;
		double droppedRel = 0.0f;

		CALIB_LOG_FILE( std::setprecision(3) << std::fixed )
		CALIB_LOG_FILE( "--- Event Cut Report ---" )
		CALIB_LOG_FILE( std::setw(20) << "CutName" << std::setw(23) << "EvtsLeftRel [%]" << std::setw(23)<< "EvtsLeft"
				<< std::setw(23)<< "EvtsDropRel [%]"<< std::setw(21) << "EvtsDropAbs")

				/*
		CALIB_LOG_FILE( std::setw(20) << "# processed events :" << std::setw(46)
		<< this->GetPipelineSettings().GetOverallNumberOfProcessedEvents() )
*/
		double precutsLetfRel = 1.0f;//(double) ( overallCountLeft ) / (double) GetPipelineSettings()->GetOverallNumberOfProcessedEvents();
		CALIB_LOG_FILE( std::setw(20) << "precuts :"
				<< std::setw(23) << std::setprecision(5) << precutsLetfRel
				<< std::setw(23) << overallCountLeft
				<< std::setw(23) << std::setprecision(5) << ( 1.0f - precutsLetfRel )
				<< std::setw(21))// << GetPipelineSettings()->GetOverallNumberOfProcessedEvents() - overallCountLeft)

		BOOST_FOREACH( ZJetPipeline::MetaDataProducerForThisPipeline * m, m_cuts )
		{
			ZJetCutBase * c = static_cast<ZJetCutBase *>( m );
			unsigned long rejAbs = m_cutRejected[c->GetCutShortName()];

/*			if ( ZJetCutHandler::IsCutEnabled( c, this->GetPipelineSettings() ) )
			{*/
				droppedRel = 1.0f -(double) ( overallCountLeft - rejAbs ) / (double) overallCountLeft;

				CALIB_LOG_FILE(std::setw(20) << c->GetCutShortName() << " : "
						<< std::setw(20) << std::setprecision(5) << (1.0f - droppedRel) * 100.0f
						<< std::setw(20) << overallCountLeft - rejAbs
						<< std::setw(20) << std::setprecision(5) << droppedRel * 100.0f
						<< std::setw(20) << rejAbs)

				overallCountLeft -= rejAbs;
		/*	}
			else
			{
				CALIB_LOG_FILE(std::setw(20) << c->GetCutShortName() << " : disabled")
			}*/
		}
		CALIB_LOG_FILE( "Events left after Cuts : " << overallCountLeft )

		CALIB_LOG_FILE( "-- Cut correlations --" )
		CALIB_LOG_FILE( "P( DeltaPhi | 2ndJetCut ) = "
				<< ( (float) m_conditional2ndJetPtCut/ (float ) m_conditional2ndJetPtCutBase) << " [ "
				<< m_conditional2ndJetPtCut << ", " << m_conditional2ndJetPtCutBase << " ]")
				CALIB_LOG_FILE( "P( 2ndJetCut | DeltaPhi ) = "
						<< ( (float) m_conditionalDeltaPhiCut/ (float ) m_conditionalDeltaPhiCutBase) << " [ "
						<< m_conditionalDeltaPhiCut << ", " << m_conditionalDeltaPhiCutBase << " ]")
	}

	// this method is only called for events which have passed the filter imposed on the
	// pipeline
	virtual void ProcessFilteredEvent(ZJetEventData const& event, ZJetMetaData const& metaData)
	{

	}

	// this method is called for all events
	virtual void ProcessEvent(ZJetEventData const& event,
			ZJetMetaData const& metaData,
			FilterResult & result)
	{
		// only look at the event if it passed all filters, except the incut filter
		// this thing has to be run on a pipeline which does not have an incut filter !!

		if ( ! result.HasPassedIfExcludingFilter( InCutFilter::Id() ) )
			return;

		m_eventCount++;
		BOOST_FOREACH( ZJetPipeline::MetaDataProducerForThisPipeline * m, m_cuts)
		{
			ZJetCutBase * c = static_cast<ZJetCutBase *>( m );

			if ( ! metaData.IsCutPassed( c->GetId() ) )
			{
				m_cutRejected[ c->GetCutShortName() ]++;
				// we only want to store the number of events, which were effectively kicked by one
				// cut here
				break;
			}
		}
/*
		if (metaData.IsCutPassed( SecondLeadingToZPtCut::CudId ))
		{
			m_conditional2ndJetPtCutBase++;
			if ( metaData.IsCutPassed( BackToBackCut::CudId ))
				m_conditional2ndJetPtCut++;
		}

		if ( metaData.IsCutPassed( BackToBackCut::CudId ))
		{
			m_conditionalDeltaPhiCutBase++;
			if ( metaData.IsCutPassed(SecondLeadingToZPtCut::CudId ))
				m_conditionalDeltaPhiCut++;
		}*/

	}


	std::map<std::string, unsigned long> m_cutRejected;
	ZJetPipeline::MetaDataProducerVector m_cuts;

	unsigned long m_conditional2ndJetPtCut;
	unsigned long m_conditional2ndJetPtCutBase;

	unsigned long m_conditionalDeltaPhiCut;
	unsigned long m_conditionalDeltaPhiCutBase;

	unsigned long m_eventCount;
};

}

