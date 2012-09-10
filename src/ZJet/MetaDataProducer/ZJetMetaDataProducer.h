#pragma once

// from ROOT
#include <Math/VectorUtil.h>

#include "../ZJetPipeline.h"

namespace CalibFW
{

//const double g_kZmass = 91.19;

typedef GlobalMetaDataProducerBase<ZJetEventData, ZJetMetaData, ZJetPipelineSettings>
		ZJetGlobalMetaDataProducerBase;
//typedef CutHandler<ZJetEventData , ZJetPipelineSettings > ZJetCutHandler;

//typedef EventConsumerBase<EventResult, ZJetPipelineSettings> ZJetConsumerBase;

class ValidMuonProducer: public ZJetGlobalMetaDataProducerBase
{
public:

	virtual bool PopulateGlobalMetaData(ZJetEventData const& data,
			ZJetMetaData & metaData, ZJetPipelineSettings const& globalSettings) const
	{
		// Apply muon isolation and MuonID
		for (KDataMuons::iterator it = data.m_muons->begin();
				it != data.m_muons->end(); it++)
		{
			bool good_muon = true;

			// Own loose cuts on muons
			good_muon = good_muon
				&& it->isGlobalMuon()
				&& it->p4.Pt() > 12.0				// 20.0
				&& std::abs(it->p4.Eta()) < 5.0		// 2.4
				&& it->sumPtIso03 < 3.0;
				// && (it->sumPtIso03 + it->hcalIso03 + it->ecalIso03) / it->p4.pt() < 0.15

			// Tight MuonID 2012
			// https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideMuonId#Tight_Muon
			// comments as CMSSW treats a recoMu
			good_muon = good_muon
				&& it->isGlobalMuon()
				// normalizedChi2
				&& it->globalTrack.chi2 / it->globalTrack.nDOF < 10.
				// hitPattern().numberOfValidMuonHits
				&& it->globalTrack.nValidMuonHits > 0
				// numberOfMatchedStations
				&& it->numberOfMatches > 1
				// fabs(muonBestTrack()->dxy(vertex->position))
				// The BestTrack is not available in Kappa, innerTrack is used
				&& std::abs(it->innerTrack.getDxy(&(data.m_vertexSummary->pv))) < 0.2
				// fabs(muonBestTrack()->dz(vertex->position))
				&& std::abs(it->innerTrack.getDz(&data.m_vertexSummary->pv)) < 0.5
				// hitPattern().numberOfValidPixelHits()
				&& it->innerTrack.nValidPixelHits > 0
				// hitPattern().trackerLayersWithMeasurement()
				&& it->track.nPixelLayers + it->track.nStripLayers > 5;

			if (good_muon)
				metaData.m_listValidMuons.push_back(*it);
			else
				metaData.m_listInvalidMuons.push_back(*it);
		}

		return true;
	}

	static std::string Name() { return "valid_muon_producer"; }
};

class ValidJetProducer: public ZJetGlobalMetaDataProducerBase
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
			// init collections for this algorithm
			std::string sAlgoName = italgo->first;
			metaData.m_listValidJets.insert(sAlgoName, new std::vector<unsigned int> ());
			metaData.m_listInvalidJets.insert(sAlgoName, new std::vector<unsigned int> ());

			int i = 0;
			float lastpt = -1.0f;
			for (KDataPFJets::iterator itjet = italgo->second->begin();
					itjet != italgo->second->end(); ++itjet)
			{
				bool good_jet = true;

				// Muon isolation DeltaR > 0.5
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
				good_jet = good_jet && (dr1 > 0.5) && (dr2 > 0.5);

				// Ensure the jets are ordered by pt!
				if ((lastpt > 0.0f) && (lastpt < itjet->p4.Pt()))
					std::cout << "Jet pt unsorted " << lastpt << " to "
							<< itjet->p4.Pt() << std::endl;

				lastpt = itjet->p4.Pt();

				// JetID
				// https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID
				// PFJets, all eta
				good_jet = good_jet
					&& (itjet->neutralHadFraction + itjet->HFHadFraction < 0.99)
					&& (itjet->neutralEMFraction < 0.99)
					&& (itjet->nConst > 1);
				// PFJets, |eta| < 2.4 (tracker)
				if (std::abs(itjet->p4.eta()) < 2.4)
				{
					good_jet = good_jet
						&& (itjet->chargedHadFraction > 0.0)
						&& (itjet->nCharged > 0)
						&& (itjet->chargedEMFraction < 0.99);
				}

				if (good_jet)
					metaData.m_listValidJets[italgo->first].push_back(i);
				else
					metaData.m_listInvalidJets[italgo->first].push_back(i);
				i++;
			}

		}

		return true;

	}

	static std::string Name() { return "valid_jet_producer"; }
};

// need the ValidMuonProducer before
class ZProducer: public ZJetGlobalMetaDataProducerBase
{
public:
    const double zmassRangeMin;
    const double zmassRangeMax;

    ZProducer() : ZJetGlobalMetaDataProducerBase(),
        zmassRangeMin( 68.0), zmassRangeMax ( 112.0 )
        {


        }

	virtual bool PopulateGlobalMetaData(ZJetEventData const& data,
			ZJetMetaData & metaData, ZJetPipelineSettings const& globalSettings) const
	{
	    KDataMuons const& valid_muons =  metaData.GetValidMuons();

		if  ( valid_muons.size() < 2)
		{
			// no Z to produce here
			metaData.SetValidZ(false);
			return false;
		}

		// old code uses	Z_mass_cut = "60.0 < mass < 120.0"
        std::vector < KDataLV > z_cand;

        if (valid_muons.size() > 3)
		{
			CALIB_LOG( "Opps. 4 valid muons ? Skipping Event")
			metaData.SetValidZ(false);
			return false;
		}


        // create all possible Z combinations
        // note: if we have more than 3 muons in an event, this may produce double counting
        for ( unsigned int i = 0; i < valid_muons.size() ; ++ i )
        {
            for ( unsigned int j = i + 1; j < valid_muons.size(); ++ j)
            {
                //std::cout << std::endl << "Combining muon " << i << " with " << j;

                KDataMuon const& m1 = valid_muons.at(i);
                KDataMuon const& m2 = valid_muons.at(j);

           		if ((m1.charge + m2.charge) == 0)
                {
                    KDataLV z;
                    //z.p4.Se
                    z.p4 = (m1.p4 + m2.p4);

                    if (  ( z.p4.mass() > zmassRangeMin ) &&
                          ( z.p4.mass() < zmassRangeMax ) )
                    {
                        z_cand.push_back( z );
                        // std::cout << std::endl << "Found possible Z with mass " << z.p4.mass();
                    }
                    else
                    {
                        // std::cout << std::endl << "Dropping Z because of wrong mass " << z.p4.mass() ;
                    }
                }
            }
        }

		if (z_cand.size() > 1)
		{
			CALIB_LOG( " -- more than 1 ZBoson candidate found " << z_cand.size() << " in an event. Is there a Higgs ? how to combine this -- dropping event for now")
			// drop this event for now
			metaData.SetValidZ(false);
			return false;
		}

		if (z_cand.size() == 0)
		{
			metaData.SetValidZ(false);
			return false;
		}


		metaData.SetZ(z_cand[0]);
		metaData.SetValidZ(true);

        return true;
	}

	static std::string Name() { return "z_producer"; }
};


}
