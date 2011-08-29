#pragma once

#include <root/Math/VectorUtil.h>

#include "EventData.h"
#include "../ZJetPipeline.h"

namespace CalibFW
{

//const double g_kZmass = 91.19;

typedef MetaDataProducerBase<ZJetEventData, ZJetMetaData, ZJetPipelineSettings>
ZJetMetaDataProducerBase;
//typedef CutHandler<ZJetEventData , ZJetPipelineSettings > ZJetCutHandler;

//typedef EventConsumerBase<EventResult, ZJetPipelineSettings> ZJetConsumerBase;

class ValidMuonProducer: public ZJetMetaDataProducerBase
{
public:
	virtual void PopulateMetaData(ZJetEventData const& data,
			ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings)
	{
		// appy muon isolation and fit quality
		for (KDataMuons::iterator it = data.m_muons->begin(); it
		!= data.m_muons->end(); it++)
		{
			// more on muon isolation here !!!
			// apply cuts here

			if (it->isGlobalMuon() && (it->sumPtIso03 < 3.0f) )
			{
				metaData.m_listValidMuons.push_back(*it);
			}
			else
			{
				metaData.m_listInvalidMuons.push_back(*it);
			}
		}
	}
};

class ValidJetProducer: public ZJetMetaDataProducerBase
{
public:
	virtual void PopulateMetaData(ZJetEventData const& event,
			ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings)
	{
		// appy PF Jet id
		unsigned int jc = event.GetJetCount(m_pipelineSettings);

		float lastpt = 0.0f;

		for (unsigned int i = 0; i < jc; i++)
		{
			KDataLV * jet = event.GetJet(m_pipelineSettings,  i);


			float dr1, dr2;

			dr1 = 99999.0f;
			dr2 = 99999.0f;

			if ( metaData.HasValidZ() )
			{
				dr1= ROOT::Math::VectorUtil::DeltaR( jet->p4, metaData.GetValidMuons().at(0).p4 );
				dr2= ROOT::Math::VectorUtil::DeltaR( jet->p4, metaData.GetValidMuons().at(1).p4 );
			}

			if (m_pipelineSettings.IsPF())
			{
				/*
				 * JetID: acoording to https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID
				 *
				 Neutral Hadron Fraction 	<0.99
				 Neutral EM Fraction 	<0.99
				 Number of Constituents 	>1*/

				KDataPFJet * pfJet = static_cast<KDataPFJet*> (jet);

				// ensure the jets are ordered by pt !
				if ( lastpt > 0.0f)
				{
					if (lastpt < pfJet->p4.Pt())
						std::cout << "Jet pt unsorted " << lastpt << " to "  << pfJet->p4.Pt() << std::endl;
				}

				lastpt = pfJet->p4.Pt();

				if ( pfJet == NULL)
				{
					std::cout << "OOPS: invalid jet " << i << ", what is up here ? Event " << event.m_eventmetadata->nEvent << std::endl;
					metaData.m_listInvalidJets.push_back(i);
				}
				else
				{
					if ((pfJet->neutralHadFraction < 0.99)
							&& (pfJet->neutralEMFraction < 0.99)
							&& (pfJet->nConst > 1)
							&& ( dr1 > 0.1 ) && ( dr2 > 0.1 ) // to be sure to exclude the PF Jets originating from muons.
							)
					{
						//is valid
						metaData.m_listValidJets.push_back(i);
					}
					else
					{
						metaData.m_listInvalidJets.push_back(i);
					}
				}
			}
			if (m_pipelineSettings.IsCalo())
			{
				CALIB_LOG_FATAL("not supported")
			}
		}
	}
};

// need the ValidMuonProducer before
class ZProducer: public ZJetMetaDataProducerBase
{
public:
	virtual void PopulateMetaData(ZJetEventData const& data,
			ZJetMetaData & metaData,
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
			//			CALIB_LOG_FATA( " -- more than 2 muons in an event, spookey ? how to combine this --")
			// drop this event for now
			metaData.SetValidZ(false);
			return;
		}

		KDataMuon const& m1 = metaData.GetValidMuons().at(0);
		KDataMuon const& m2 = metaData.GetValidMuons().at(1);

		if ((m1.charge + m2.charge) != 0)
		{
			metaData.SetValidZ(false);
			return;
		}

		// quality cuts on muon go here
		KDataLV z;
		//z.p4.Se
		z.p4 = (m1.p4 + m2.p4);
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
