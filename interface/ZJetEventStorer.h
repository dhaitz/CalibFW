/*
#pragma once

#include <string>
#include <iostream>
#include <sstream>

#include <stdio.h>
#include <stdlib.h>

#include <boost/ptr_container/ptr_list.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>



#include <typeinfo>

#include "GlobalInclude.h"
#include "RootIncludes.h"
#include "EventData.h"
#include "PtBinWeighter.h"
#include "ZJetPipeline.h"
#include "ZJetConsumer.h"

namespace CalibFW
{

class EventDump: public ZJetConsumerBase
{
public:
	virtual void Init(ZJetPipeline * pset)
	{
		CALIB_LOG("EventDump::Init called")
	}

	virtual void Finish()
	{
		CALIB_LOG("EventDump::Finish called")
	}

	// this method is only called for events which have passed the filter imposed on the
	// pipeline
	virtual void ProcessFilteredEvent(ZJetEventData & event, ZJetMetaData const& metaData)
	{
		// todo
		//CALIB_LOG("Filtered EventNum " << event.m_pData->cmsEventNum)
	}

	// this method is called for all events
	virtual void ProcessEvent(ZJetEventData & event, ZJetMetaData const& metaData, FilterResult & result)
	{
		// todo
		// CALIB_LOG("Unfiltered EventNum " << event.m_pData->cmsEventNum)
	}
};

class EventStorerConsumer: public ZJetConsumerBase
{
public:
	virtual void Init(ZJetPipeline * pset)
	{
		ZJetConsumerBase::Init( pset );

		m_l2corr = 1.0f;
	 	m_l2corrPtJet2 = 1.0f;
	 	m_l2corrPtJet3 = 1.0f;
		 m_gentree = new TTree( (this->GetPipelineSettings()->GetAlgoName() + "_events").c_str(), 
			(this->GetPipelineSettings()->GetAlgoName() + "_events").c_str());

	 	m_localData.jets[0] = new TParticle();
		m_localData.jets[1] = new TParticle();
		m_localData.jets[2] = new TParticle();
		m_localData.met = new TParticle();
	 	m_localData.Z = new TParticle();
	 	m_localData.mu_plus = new TParticle();
	 	m_localData.mu_minus = new TParticle();

		 // more data can go here
		 m_gentree->Branch("Z", "TParticle", &m_localData.Z);
		 m_gentree->Branch("matched_Z", "TParticle", &m_localData.matched_Z);
		 m_gentree->Branch("mu_plus", "TParticle", &m_localData.mu_plus);
		 m_gentree->Branch("mu_minus", "TParticle", &m_localData.mu_minus);
		 m_gentree->Branch("jet1", "TParticle", &m_localData.jets[0]);
		 m_gentree->Branch("jet2", "TParticle", &m_localData.jets[1]);
		 m_gentree->Branch("jet3", "TParticle", &m_localData.jets[2]);
		 m_gentree->Branch("met", "TParticle", &m_localData.met);
		 m_gentree->Branch("l2corrJet", &m_l2corr, "l2corrJet/D");
		 m_gentree->Branch("l2corrPtJet2", &m_l2corrPtJet2, "l2corrPtJet2/D");
		 m_gentree->Branch("l2corrPtJet3", &m_l2corrPtJet3, "l2corrPtJet3/D");

		 m_gentree->Branch("cmsEiventNum", &m_localData.cmsEventNum, "cmsEventNum/L");
		 m_gentree->Branch("cmsRun", &m_localData.cmsRun, "cmsRun/L");
		 m_gentree->Branch("luminosityBlock", &m_localData.luminosityBlock, "cmsRun/L");
		m_gentree->Branch("eventWeight", &m_localData.weight, "eventWeight/D");

		std::stringstream sout;
		m_evtFormater.Header ( sout );
		CALIB_LOG_FILE( sout.str() )
	}

	virtual void Finish()
	{
		 //this->GetPipelineSettings()->GetRootOutFile()->cd( this->GetPipelineSettings()->GetRootFileFolder().c_str() );

		// store to root for now
		this->GetPipelineSettings()->GetRootOutFile()->cd( );
		 m_gentree->Write();
	}

	virtual std::string GetId()
	{
		return "event_storer";
	}

	// this method is only called for events which have passed the filter imposed on the
	// pipeline
	virtual void ProcessFilteredEvent(ZJetEventData const& event, ZJetMetaData const& metaData)
	{

		//todo
		 m_gentree->Fill();

		 //std::stringstream sout;
		 //m_evtFormater.FormatEventResultCorrected(sout, &event);

		 //CALIB_LOG_FILE( sout.str() )
	}

	 evtData m_localData;
	 Double_t m_l2corr;
	 Double_t m_l2corrPtJet2;
	 Double_t m_l2corrPtJet3;
	TTree* m_gentree;
	EventFormater m_evtFormater;
};

}
*/
