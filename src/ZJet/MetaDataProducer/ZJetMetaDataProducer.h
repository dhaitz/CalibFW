#pragma once

// from ROOT
#include <Math/VectorUtil.h>
#include "../ZJetPipeline.h"

namespace CalibFW
{

typedef GlobalMetaDataProducerBase<ZJetEventData, ZJetMetaData, ZJetPipelineSettings>
		ZJetGlobalMetaDataProducerBase;
//typedef CutHandler<ZJetEventData , ZJetPipelineSettings > ZJetCutHandler;
//typedef EventConsumerBase<EventResult, ZJetPipelineSettings> ZJetConsumerBase;

/** Select only valid muons.

    The muons are valid if the pass these criteria
    1. soft cuts on muons
    1. muon isolation
    1. tight muonID
*/
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

			// Own loose cuts on muons and muon isolation
			good_muon = good_muon
				&& it->p4.Pt() > 12.0
				&& std::abs(it->p4.Eta()) < 5.0
				&& it->sumPtIso03 < 3.0;

			// Tight MuonID 2012
			// [twiki](https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideMuonId#Tight_Muon)
			// The comments describe, how CMSSW treats the recoMu.
			/// version of MuonID
			bool is2011 = globalSettings.Global()->GetMuonID2011();
			good_muon = good_muon
				&& it->isGlobalMuon()
				// use PF muons
				&& it->isPFMuon()
				// normalizedChi2
				&& it->globalTrack.chi2 / it->globalTrack.nDOF < 10.
				// hitPattern().numberOfValidMuonHits
				&& it->globalTrack.nValidMuonHits > 0
				// numberOfMatchedStations
				&& it->numberOfMatches > 1
				// fabs(muonBestTrack()->dxy(vertex->position))
				&& std::abs(it->bestTrack.getDxy(&data.m_vertexSummary->pv)) < 0.2
				// fabs(muonBestTrack()->dz(vertex->position)) // not in 2011
				&& std::abs(it->bestTrack.getDz(&data.m_vertexSummary->pv)) < 0.5 + 99999. * is2011
				// hitPattern().numberOfValidPixelHits()
				&& it->innerTrack.nValidPixelHits > 0
				// hitPattern().trackerLayersWithMeasurement() // 8 in 2011
				&& it->track.nPixelLayers + it->track.nStripLayers > 5 + 3 * is2011;

			if (good_muon)
				metaData.m_listValidMuons.push_back(*it);
			else
				metaData.m_listInvalidMuons.push_back(*it);
		}

		return true;
	}

	static std::string Name() { return "valid_muon_producer"; }
};


/** Select only valid jets.

    The jets are valid if the pass these criteria
    1. muon isolation
    1. jetID
*/
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
					CALIB_LOG("Jet pt unsorted " << lastpt << " to " << itjet->p4.Pt())

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


/** Combine valid muons to a Z boson.

    There must be 2 or 3 valid muons that fullfil the Z requirements.
    This requires the @see ValidMuonProducer before.
*/
class ZProducer: public ZJetGlobalMetaDataProducerBase
{
public:

	ZProducer() : ZJetGlobalMetaDataProducerBase(),
		zmassRangeMin(71.19), zmassRangeMax(111.19)
	{}

	virtual bool PopulateGlobalMetaData(ZJetEventData const& data,
			ZJetMetaData & metaData, ZJetPipelineSettings const& globalSettings) const
	{
		KDataMuons const& valid_muons = metaData.GetValidMuons();

		if (valid_muons.size() < 2)
		{
			// no Z to produce here
			metaData.SetValidZ(false);
			return false;
		}

		// old code uses	Z_mass_cut = "60.0 < mass < 120.0"
		std::vector<KDataLV> z_cand;

		if (valid_muons.size() > 3)
		{
			CALIB_LOG("Warning: 4 valid muons? Skipping Event.")
			metaData.SetValidZ(false);
			return false;
		}


		// Create all possible Z combinations
		// Note: If we have more than 3 muons in an event, this may produce double counting
		for (unsigned int i = 0; i < valid_muons.size() ; ++i)
		{
			for (unsigned int j = i + 1; j < valid_muons.size(); ++j)
			{
				KDataMuon const& m1 = valid_muons.at(i);
				KDataMuon const& m2 = valid_muons.at(j);

				if (m1.charge + m2.charge == 0)
				{
					KDataLV z;
					z.p4 = m1.p4 + m2.p4;

					if (z.p4.mass() > zmassRangeMin && z.p4.mass() < zmassRangeMax)
					{
						z_cand.push_back(z);
						//CALIB_LOG("Found possible Z with mass " << z.p4.mass())
					}
					else
					{
						//CALIB_LOG("Dropping Z because of wrong mass " << z.p4.mass())
					}
				}
			}
		}

		if (z_cand.size() > 1)
		{
			CALIB_LOG(" -- more than 1 Z boson candidate found." << z_cand.size() << " in an event. Not sure how to combine this. Dropping event for now.")
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

private:
	const double zmassRangeMin;
	const double zmassRangeMax;
};


/** Produce lists of different generator particle categories.

    These particles are treated (status == 3):
    pdgId  Name
    13     µ
    23     Z
    1-6,21 q,g
    2212   p

    This requires no other producer.
    Monte-Carlo only.
*/
class GenProducer: public ZJetGlobalMetaDataProducerBase
{
public:

	GenProducer(): ZJetGlobalMetaDataProducerBase(),
		nmin(50), nmax(1300)
	{}

	virtual bool PopulateGlobalMetaData(ZJetEventData const& data,
			ZJetMetaData & metaData, ZJetPipelineSettings const& globalSettings) const
	{
		// Check number of particles (could be simplified after study)
		if (data.m_particles->size() < 0)
			CALIB_LOG("This event contains no generator information.")
		else if (data.m_particles->size() < nmin)
			CALIB_LOG("This event contains only few particles: " << data.m_particles->size())
		else if (data.m_particles->size() > nmax)
			CALIB_LOG("This event contains a lot of particles: " << data.m_particles->size())

		// Loop over particles
		for (auto it = data.m_particles->begin(); it != data.m_particles->end(); ++it)
		{
			// Take only stable final particles and check for children
			if (it->status() != 3)
				continue;
			if (it->children != 0)
				CALIB_LOG("Particle has " << it->children << " children.")

			// Sort particles in lists in metaData
			if (std::abs(it->pdgId()) == 13)		// muon
			{
				metaData.m_genMuons.push_back(*it);
			}
			else if (std::abs(it->pdgId()) == 23)	// Z
			{
				metaData.m_genZs.push_back(*it);
			}
			else if (std::abs(it->pdgId()) < 7 || std::abs(it->pdgId()) == 21)	// parton
			{
				metaData.m_genPartons.push_back(*it);
				//CALIB_LOG("parton: " << it->pdgId() << " pt=" << it->p4.Pt()<< " eta=" << it->p4.Eta())
			}
			else if (it->pdgId() == 2212 && it->p4.Pt() < 1e-6) // ignore incoming protons
			{
			}
			else // unexpected particles
			{
				CALIB_LOG("Unexpected particle with id: " << it->pdgId() << ", status: " << it->status())
			}
		}

		// check for unusual behaviour
		if (metaData.m_genZs.size() < 1)
			CALIB_LOG("There is no Z!")
		if (metaData.m_genPartons.size() < 1)
			CALIB_LOG("There is no parton!")
		if (metaData.m_genMuons.size() < 1)
			CALIB_LOG("There are no gen muons!")
		if (metaData.m_genMuons.size() > 2)
			CALIB_LOG("There are more than 2 gen muons (" << metaData.m_genMuons.size() << ")!")

		return true;
	}

	static std::string Name() { return "gen_producer"; }

private:
	const unsigned short int nmin;
	const unsigned short int nmax;
};


/* Produce the balanced objects

    The generated Z boson is required to be unique and to have a minimal pt.
    The parton must have a minimal pt and it must be close to the Z in pt and phi.
    There must be exactly 2 gen muons that match the gen Z kinematics.

    This requires the @see GenProducer before.
*/
class GenBalanceProducer: public ZJetGlobalMetaDataProducerBase
{
public:

	GenBalanceProducer(): ZJetGlobalMetaDataProducerBase(),
		zptmin(5.0), pptmin(3.0), petamax(5.0), threshold(1.9)
	{}

	virtual bool PopulateGlobalMetaData(ZJetEventData const& data,
			ZJetMetaData & metaData, ZJetPipelineSettings const& globalSettings) const
	{
		// Valid gen Z producer
		if (metaData.m_genZs.size() < 1)
		{
			CALIB_LOG("No gen Z in the event.")
			metaData.SetValidGenZ(false);
			return false;
		}
		if (metaData.m_genZs.size() > 1)
		{
			CALIB_LOG("More than one gen Z in the event.")
			// Could be resolved, but this case is currently not present.
			metaData.SetValidGenZ(false);
			return false;
		}
		if (metaData.m_genZs[0].p4.Pt() < zptmin)
		{
			//CALIB_LOG("Gen Z pt is very low, no balance calculated.")
			metaData.SetValidGenZ(false);
			return false;
		}

		// check if the Z decays to the muons:
		if (metaData.m_genMuons.size() != 2)
		{
			CALIB_LOG("Not exactly two muons in the event, therefore no valid Z.")
			metaData.SetValidGenZ(false);
			return false;
		}
		if (metaData.m_genZs.size() >= 1)
		{
			KParton test;
			test.p4 = metaData.m_genMuons[0].p4 + metaData.m_genMuons[1].p4 - metaData.m_genZs[0].p4;
			if (test.p4.Pt() > 1e-3)	// differs more than a MeV
			{
				CALIB_LOG("Muons not from Z decay: pt:" << test.p4.Pt() << ", eta: " << test.p4.Eta()<< ", phi: " << test.p4.Phi()<< ", m: " << test.p4.M())
				metaData.SetValidGenZ(false);
				return false;
			}
		}
		metaData.SetValidGenZ(true);
		metaData.SetGenZ(metaData.m_genZs[0]);

		//search
		metaData.SetBalanceQuality(threshold);
		metaData.SetValidParton(false);
		if (metaData.m_genPartons.size() < 1)
		{
			CALIB_LOG("No partons in the event!")
			return false;
		}

		double dphi = -1.;
		double R = -1.;
		double bQuality = -1.;
		KDataLV met;
		met.p4 = metaData.GetRefGenZ().p4;

		for (auto it = metaData.m_genPartons.begin(); it != metaData.m_genPartons.end(); ++it)
		{
			if (it->p4.Pt() < pptmin || it->p4.Eta() > petamax)
				continue;
			met.p4 = met.p4 + it->p4;

			dphi = ROOT::Math::VectorUtil::DeltaPhi(it->p4, metaData.GetRefGenZ().p4);
			dphi = ROOT::Math::VectorUtil::Phi_mpi_pi(dphi - ROOT::Math::Pi());
			R = it->p4.Pt() / metaData.GetRefGenZ().p4.Pt();
			// decision metric
			bQuality = std::abs(dphi) + 2.0 * std::abs(R - 1.0);

			if (bQuality < metaData.GetRefBalanceQuality())
			{
				//if (metaData.GetValidParton() && metaData.GetRefParton().p4.Pt() > it->p4.Pt())
				//{
				//	CALIB_LOG("The best balanced parton is not the leading one!")
				//	CALIB_LOG("  Best    (" << it->pdgId() << ") Q: " << bQuality << ", pt: " <<it->p4.Pt() << ", dphi: " << dphi << ", R: " << R)
				//	CALIB_LOG("  Leading (" << metaData.GetRefParton().pdgId() << ") Q: " << metaData.GetRefBalanceQuality() << ", pt: " <<metaData.GetRefParton().p4.Pt())
				//}
				metaData.SetValidParton(true);
				metaData.SetBalanceQuality(bQuality);
				metaData.SetBalancedParton(*it);
				//CALIB_LOG("Balance (" << it->pdgId() << ") dphi: " << dphi << ", R: " << R << ", Q: " << bQuality)
			}
			if (it == metaData.m_genPartons.begin()
					|| metaData.GetRefLeadingParton().p4.Pt() < it->p4.Pt())
				metaData.SetLeadingParton(*it);
		}

		if (!metaData.GetRefValidParton())
		{
			//CALIB_LOG("No balance found below threshold=" << metaData.GetRefBalanceQuality() << "!")
			return false;
		}
		//CALIB_LOG("Best parton is " << metaData.GetRefParton().pdgId() << ": " << metaData.GetRefBalanceQuality())

		metaData.SetGenMet(met);
		return true;
	}

	static std::string Name() { return "gen_balance_producer"; }

private:
	const double zptmin;
	const double pptmin;
	const double petamax;
	const double threshold;
};


/** Complex balance finder

    This includes balance of two partons to the Z. The best balanced object is
    stored in m_GenParton as the GenBalanceProducer does and thus overwrites
    the balanced object if it finds a better one.

    This requires the @see GenBalanceProducer before.
*/
class GenDibalanceProducer: public ZJetGlobalMetaDataProducerBase
{
public:

	GenDibalanceProducer(): ZJetGlobalMetaDataProducerBase()
	{}

	virtual bool PopulateGlobalMetaData(ZJetEventData const& data,
			ZJetMetaData & metaData, ZJetPipelineSettings const& globalSettings) const
	{
		double dphi = -1.;
		double R = -1.;
		double bQuality = -1.;
		// combine and look for balancing of 2 partons
		for (auto i = metaData.m_genPartons.begin(); i != metaData.m_genPartons.end(); ++i)
			for (auto j = i; j != metaData.m_genPartons.end(); ++j)
			{
				if (std::abs(j->p4.Pt() - i->p4.Pt()) < 1e-6)
					continue;
				KParton comb;
				comb.p4 = i->p4 + j->p4;
				dphi = ROOT::Math::VectorUtil::DeltaPhi(comb.p4, metaData.GetRefGenZ().p4);
				dphi = ROOT::Math::VectorUtil::Phi_mpi_pi(dphi - ROOT::Math::Pi());
				R = comb.p4.Pt() / metaData.GetRefGenZ().p4.Pt();
				// decision metric
				bQuality = std::abs(dphi) + 2.0 * std::abs(R - 1.0);

				if (bQuality < metaData.GetRefBalanceQuality()){
					metaData.SetValidParton(true);
					metaData.SetBalanceQuality(bQuality);
					if (i->p4.Pt() > j->p4.Pt())
					{
						metaData.SetBalancedParton(*i);
						//CALIB_LOG("Balance (" << i->pdgId() << ") dphi: " << dphi << ", R: " << R << ", Q: " << bQuality)
					}
					else
					{
						metaData.SetBalancedParton(*j);
						//CALIB_LOG("Balance (" << i->pdgId() << ") dphi: " << dphi << ", R: " << R << ", Q: " << bQuality)
					}
				}
			}
		return true;
	}

	static std::string Name() { return "gen_dibalance_producer"; }
};

}
