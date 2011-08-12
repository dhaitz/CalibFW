#pragma once

#include "EventData.h"
#include "CutHandler.h"
#include "ZJetPipeline.h"

namespace CalibFW
{

//const double g_kZmass = 91.19;

typedef MetaDataProducerBase<ZJetEventData , ZJetMetaData,  ZJetPipelineSettings > ZJetMetaDataProducerBase;
//typedef CutHandler<ZJetEventData , ZJetPipelineSettings > ZJetCutHandler;

//typedef EventConsumerBase<EventResult, ZJetPipelineSettings> ZJetConsumerBase;

class ZProducer: public ZJetMetaDataProducerBase
{
public:
	virtual void PopulateMetaData(ZJetEventData const& data, ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings)
	{
		if (data.m_muons->size() < 2)
		{
			metaData.SetValidMuons(false);
			return;
		}

		if (data.m_muons->size() > 2)
		{
			CALIB_LOG_FATAL( " -- more than 2 muons in an event, spookey ? how to combine this --")
		}

		KDataMuon & m1 = data.m_muons->at(0);
		KDataMuon & m2 = data.m_muons->at(1);

		if (( m1.charge + m2.charge ) != 0 )
			return;


		// quality cuts on muon go here
		KDataLV z;
		z.p4 = ( m1.p4 + m2.p4);
		metaData.SetZ(z);
		metaData.SetValidMuons( true );

	}
};


}
