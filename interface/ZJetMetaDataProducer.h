#pragma once

#include "EventData.h"

#include "ZJetPipeline.h"

namespace CalibFW
{

//const double g_kZmass = 91.19;

typedef MetaDataProducerBase<ZJetEventData , ZJetMetaData,  ZJetPipelineSettings > ZJetMetaDataProducerBase;
//typedef CutHandler<ZJetEventData , ZJetPipelineSettings > ZJetCutHandler;

//typedef EventConsumerBase<EventResult, ZJetPipelineSettings> ZJetConsumerBase;

class ValidMuonProducer: public ZJetMetaDataProducerBase
{
public:
	virtual void PopulateMetaData(ZJetEventData const& data, ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings)
	{
		// appy muon isolation and fit quality
		for ( KDataMuons::iterator it = data.m_muons->begin();
				it != data.m_muons->end(); it++ )
		{
			// more on muon isolation here !!!
			// apply cuts here

			if ( it->isGlobalMuon()  &&
					( it->sumPtIso03 < 3.0f ) )
			{
				metaData.m_listValidMuons.push_back( *it );
			}
			else
			{
				metaData.m_listInvalidMuons.push_back( *it );
			}
		}
	}
};

// need the ValidMuonProducer before
class ZProducer: public ZJetMetaDataProducerBase
{
public:
	virtual void PopulateMetaData(ZJetEventData const& data, ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings)
	{
		if (metaData.GetValidMuons().size() < 2)
		{
			// no Z to produce here
			metaData.SetValidZ(false);
			return;
		}

		if (data.m_muons->size() > 2)
		{
//			CALIB_LOG_FATAL( " -- more than 2 muons in an event, spookey ? how to combine this --")
		}

		KDataMuon const& m1 = metaData.GetValidMuons().at(0);
		KDataMuon const& m2 = metaData.GetValidMuons().at(1);

		if (( m1.charge + m2.charge ) != 0 )
			return;

		// quality cuts on muon go here
		KDataLV z;
		z.p4 = ( m1.p4 + m2.p4);
		metaData.SetZ(z);
		metaData.SetValidZ(true);

	}
};
/*
class ValidNPVProducer: public ZJetMetaDataProducerBase
{
public:
	virtual void PopulateMetaData(ZJetEventData const& data, ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings)
	{
		unsigned int n = 0;

		for( KDataVertices::iterator it = data.m_primaryVertex->begin();
			it != data.m_primaryVertex->end();
			it ++)
		{
			if ( !it->fake )
				n++;
		}
		metaData.SetValidNPV(n);
	}
};*/


}
