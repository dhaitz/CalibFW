#pragma once

// from ROOT
#include <Math/VectorUtil.h>

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
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		// nothing to do here
	}

	virtual bool PopulateGlobalMetaData(ZJetEventData const& data,
			ZJetMetaData & metaData, ZJetPipelineSettings const& globalSettings) const
	{
		// appy muon isolation and fit quality
		for (KDataMuons::iterator it = data.m_muons->begin(); it
				!= data.m_muons->end(); it++)
		{
			// more on muon isolation here !!!
			// apply cuts here

			if (it->isGlobalMuon() && (it->sumPtIso03 < 3.0f))
			{
				metaData.m_listValidMuons.push_back(*it);
			}
			else
			{
				metaData.m_listInvalidMuons.push_back(*it);
			}
		}

		return true;
	}

	static std::string Name() { return "valid_muon_producer"; }
};

class ValidJetProducer: public ZJetMetaDataProducerBase
{
public:

	virtual void PopulateMetaData(ZJetEventData const& data,
			ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		// nothing to do here
	}

	virtual bool PopulateGlobalMetaData(ZJetEventData const& event,
			ZJetMetaData & metaData,
			ZJetPipelineSettings const& globalSettings) const
	{
		// all gen jets are valid ...

		// validate PF Jets
		for (ZJetEventData::PfMapIterator italgo = event.m_pfJets.begin(); italgo
				!= event.m_pfJets.end(); ++italgo)
		{
			//init collections for this algorithm
			metaData.m_listValidJets[italgo->first];
			metaData.m_listInvalidJets[italgo->first];

			int i = 0;
			float lastpt = -1.0f;
			for (KDataPFJets::iterator itjet = italgo->second->begin(); itjet
					!= italgo->second->end(); ++itjet)
			{
				// implement calo ...
				float dr1, dr2;

				dr1 = 99999.0f;
				dr2 = 99999.0f;

				if (metaData.HasValidZ())
				{
					dr1 = ROOT::Math::VectorUtil::DeltaR(itjet->p4,
							metaData.GetValidMuons().at(0).p4);
					dr2 = ROOT::Math::VectorUtil::DeltaR(itjet->p4,
							metaData.GetValidMuons().at(1).p4);
				}
				// JetID: acoording to https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID
				///Neutral Hadron Fraction 	<0.99
				///Neutral EM Fraction 	<0.99
				//Number of Constituents 	>1

				// ensure the jets are ordered by pt !
				if (lastpt > 0.0f)
				{
					if (lastpt < itjet->p4.Pt())
						std::cout << "Jet pt unsorted " << lastpt << " to "
								<< itjet->p4.Pt() << std::endl;
				}

				lastpt = itjet->p4.Pt();

				// barrel
				bool good_jet = (itjet->neutralHadFraction < 0.99)
						&& (itjet->neutralEMFraction < 0.99) && (itjet->nConst
						> 1)
				// to be sure to exclude the PF Jets originating from muons.
						&& (dr1 > 0.1) && (dr2 > 0.1);

				//Add criteria for PF jets with eta > 2.4 according to https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID
				if (TMath::Abs(itjet->p4.eta()) >= 2.4)
				{
					good_jet = good_jet && (itjet->chargedHadFraction > 0)
							&& (itjet->nCharged > 0)
							&& (itjet->chargedEMFraction < 0.99);
				}
				if (good_jet)
				{
					//is valid
					metaData.m_listValidJets[italgo->first].push_back(i);
				}
				else
				{
					metaData.m_listInvalidJets[italgo->first].push_back(i);
				}

				i++;
			}

		}

		return true;

	}

	static std::string Name() { return "valid_jet_producer"; }
};

// need the ValidMuonProducer before
class ZProducer: public ZJetMetaDataProducerBase
{
public:
	virtual void PopulateMetaData(ZJetEventData const& data,
			ZJetMetaData & metaData,
			ZJetPipelineSettings const& m_pipelineSettings) const
	{
		// nothing to do here
	}

	virtual bool PopulateGlobalMetaData(ZJetEventData const& data,
			ZJetMetaData & metaData, ZJetPipelineSettings const& globalSettings) const
	{
		if (metaData.GetValidMuons().size() < 2)
		{
			// no Z to produce here
			metaData.SetValidZ(false);
			return false;
		}

		if (data.m_muons->size() > 2)
		{
			//			CALIB_LOG_FATA( " -- more than 2 muons in an event, spookey ? how to combine this --")
			// drop this event for now
			metaData.SetValidZ(false);
			return false;
		}

		KDataMuon const& m1 = metaData.GetValidMuons().at(0);
		KDataMuon const& m2 = metaData.GetValidMuons().at(1);

		if ((m1.charge + m2.charge) != 0)
		{
			metaData.SetValidZ(false);
			return false;
		}

		// quality cuts on muon go here
		KDataLV z;
		//z.p4.Se
		z.p4 = (m1.p4 + m2.p4);
		metaData.SetZ(z);
		metaData.SetValidZ(true);

        return true;
	}

	static std::string Name() { return "z_producer"; }
};


}
