#pragma once

// from ROOT
#include <Math/VectorUtil.h>
#include "ZJetEventPipeline/Pipeline.h"

namespace Artus
{

typedef GlobalMetaDataProducerBase<ZJetEventData, ZJetMetaData, ZJetPipelineSettings>
ZJetGlobalMetaDataProducerBase;

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
										ZJetMetaData& metaData,
										ZJetPipelineSettings const& globalSettings) const
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
						&& it->trackIso03 < 3.0;

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
						&& it->nMatches > 1
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

	static std::string Name()
	{
		return "valid_muon_producer";
	}
};


/** Select only valid jets.

    The jets are valid if the pass these criteria
    1. muon isolation
    1. jetID
*/
class ValidJetProducer: public ZJetGlobalMetaDataProducerBase
{

public:
	ValidJetProducer(bool Tagged) : ZJetGlobalMetaDataProducerBase(), tagged(Tagged), muonIso(true) {}
	ValidJetProducer(bool Tagged, bool MuonIso) : ZJetGlobalMetaDataProducerBase(), tagged(Tagged), muonIso(MuonIso) {}

	virtual void PopulateMetaData(ZJetEventData const& data,
								  ZJetMetaData& metaData,
								  ZJetPipelineSettings const& m_pipelineSettings) const
	{
		// nothing to do here
	}

	virtual bool PopulateGlobalMetaData(ZJetEventData const& event,
										ZJetMetaData& metaData,
										ZJetPipelineSettings const& globalSettings) const
	{

		// for tagged PF jets: fill the m_pfPointerJets with a collection of
		// casted KDataPFJet pointers pointing to the KDataPFTaggedJet
		if (tagged)
		{
			for (ZJetEventData::PfTaggedMapIterator italgo = event.m_pfTaggedJets.begin();
				 italgo != event.m_pfTaggedJets.end(); ++italgo)
			{
				std::string sAlgoNameTagged = italgo->first;
				std::string sAlgoName = sAlgoNameTagged;
				sAlgoName.replace(sAlgoName.find("TaggedJets"), 10, "Jets");

				event.m_pfPointerJets[sAlgoName] = new std::vector<KDataPFJet*>;


				KDataPFTaggedJets* tjets = event.m_pfTaggedJets.at(sAlgoNameTagged);
				event.m_pfPointerJets[sAlgoName] = new std::vector<KDataPFJet*>;
				for (KDataPFTaggedJets::iterator it = tjets->begin(); it != tjets->end(); ++it)
					event.m_pfPointerJets[sAlgoName]->push_back(dynamic_cast<KDataPFJet*>(&(*it)));
			}
		}
		// for regular (non-tagged) PF jets: fill the m_pfPointerJets with simple pointers
		else
		{
			for (ZJetEventData::PfMapIterator italgo = event.m_pfJets.begin();
				 italgo != event.m_pfJets.end(); ++italgo)
			{
				std::string sAlgoName = italgo->first;
				event.m_pfPointerJets[sAlgoName] = new std::vector<KDataPFJet*>;

				KDataPFJets* tjets = event.m_pfJets.at(sAlgoName);
				event.m_pfPointerJets[sAlgoName] = new std::vector<KDataPFJet*>;
				for (KDataPFJets::iterator it = tjets->begin(); it != tjets->end(); ++it)
					event.m_pfPointerJets[sAlgoName]->push_back(&(*it));
			}
		}

		// validate PF Jets
		for (ZJetEventData::PfPointerMapIterator italgo = event.m_pfPointerJets.begin(); italgo
			 != event.m_pfPointerJets.end(); ++italgo)
		{
			// init collections for this algorithm
			std::string sAlgoName = italgo->first;
			metaData.m_listValidJets.insert(sAlgoName, new std::vector<unsigned int> ());
			metaData.m_listInvalidJets.insert(sAlgoName, new std::vector<unsigned int> ());

			int i = 0;

			for (std::vector<KDataPFJet*>::iterator itjet = italgo->second->begin();
				 itjet != italgo->second->end(); ++itjet)
			{
				bool good_jet = true;

				// Muon isolation DeltaR > 0.5
				if (muonIso)
				{
					float dr1, dr2;
					dr1 = 99999.0f;
					dr2 = 99999.0f;

					if (metaData.HasValidZ())
					{
						dr1 = ROOT::Math::VectorUtil::DeltaR((*itjet)->p4,
															 metaData.GetValidMuons().at(0).p4);
						dr2 = ROOT::Math::VectorUtil::DeltaR((*itjet)->p4,
															 metaData.GetValidMuons().at(1).p4);
					}
					good_jet = good_jet && (dr1 > 0.5) && (dr2 > 0.5);
				}

				// JetID
				// https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID
				// PFJets, all eta
				good_jet = good_jet
						   && (*itjet)->neutralHadFraction + (*itjet)->HFHadFraction < 0.99
						   && (*itjet)->neutralEMFraction < 0.99
						   && (*itjet)->nConst > 1;
				// PFJets, |eta| < 2.4 (tracker)
				if (std::abs((*itjet)->p4.eta()) < 2.4)
				{
					good_jet = good_jet
							   && (*itjet)->chargedHadFraction > 0.0
							   && (*itjet)->nCharged > 0
							   && (*itjet)->chargedEMFraction < 0.99;
				}

				if (globalSettings.Global()->GetVetoPileupJets())
				{
					bool puID = static_cast<KDataPFTaggedJet*>(*itjet)->getpuJetID("PUJetIDFullMedium", event.m_taggermetadata);
					good_jet = good_jet && puID;
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

	static std::string Name()
	{
		return "valid_jet_producer";
	}
	bool tagged;
	bool muonIso;
};

/** Select only valid jets (FOR ELECTRON ANALYSIS).

    Same as ValidJetProducer but without muon isolation.
*/
class ValidJetEEProducer: public ValidJetProducer
{
public:
	ValidJetEEProducer(bool Tagged) : ValidJetProducer(Tagged, false) {}

	static std::string Name()
	{
		return "valid_jet_ee_producer";
	}
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
										ZJetMetaData& metaData,
										ZJetPipelineSettings const& globalSettings) const
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
			LOG("Warning: 4 valid muons? Skipping Event.")
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
						//LOG("Found possible Z with mass " << z.p4.mass())
					}
					else
					{
						//LOG("Dropping Z because of wrong mass " << z.p4.mass())
					}
				}
			}
		}

		if (z_cand.size() > 1)
		{
			LOG(" -- more than 1 Z boson candidate found." << z_cand.size() << " in an event. Not sure how to combine this. Dropping event for now.")
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

	static std::string Name()
	{
		return "z_producer";
	}

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
		nmin(50), nmax(2500)
	{}

	virtual bool PopulateGlobalMetaData(ZJetEventData const& data,
										ZJetMetaData& metaData, ZJetPipelineSettings const& globalSettings) const
	{
		// Check number of particles (could be simplified after study)
		if (data.m_particles->size() < 0)
		{
			LOG("This event contains no generator information.");
		}
		else if (data.m_particles->size() < nmin)
		{
			LOG("This event contains only few particles: " << data.m_particles->size());
		}
		else if (data.m_particles->size() > nmax)
		{
			LOG("This event contains a lot of particles: " << data.m_particles->size());
		}

		const float R = 0.5; // for DeltaR matching

		// Loop over particles
		for (auto it = data.m_particles->begin(); it != data.m_particles->end(); ++it)
		{
			// Take only stable final particles
			//if (it->status() != 3)
			//    continue;
			//LOG("Particle " << *it);
			// ignore first 6 particles (because that is what CMSSW does)
			if (it - data.m_particles->begin() < 6)
				continue;

			if (unlikely(it->children != 0))
				LOG("Particle has " << it->children << " children.");
			//if (std::abs(it->pdgId()) == 13) LOG("P " <<*it);
			// Sort particles in lists in metaData
			if (std::abs(it->pdgId()) == 13 && it->status() == 1)		// stable muon
			{
				metaData.m_genMuons.push_back(*it);
			}
			else if (std::abs(it->pdgId()) == 13 && it->status() == 2)		// intermediate muon
			{
				metaData.m_genIntermediateMuons.push_back(*it);
			}
			else if (std::abs(it->pdgId()) == 13 && it->status() == 3)		// internal muon
			{
				metaData.m_genInternalMuons.push_back(*it);
			}
			else if (std::abs(it->pdgId()) == 23 && it->status() == 2)	// Z
			{
				metaData.m_genZs.push_back(*it);
			}
			else if (std::abs(it->pdgId()) == 22 && it->status() == 1)	// photon
			{
				metaData.m_genPhotons.push_back(*it);
			}
			else if (std::abs(it->pdgId()) < 7 || std::abs(it->pdgId()) == 21)	// parton
			{
				metaData.m_genPartons.push_back(*it);
			}
			else if (it->pdgId() == 2212 && it->p4.Pt() < 1e-6) // ignore incoming protons
			{
			}
			else if (std::abs(it->pdgId()) == 11 || std::abs(it->pdgId()) == 15) // ignore electrons and taus
			{
			}
			else if (it->status() == 1 &&
					 ((std::abs(it->pdgId()) == 12)
					  || (std::abs(it->pdgId()) == 14)
					  || (std::abs(it->pdgId()) == 16)))  // neutrinos
			{
				for (ZJetEventData::GenJetMapIterator it2 = data.m_genJets.begin(); it2 != data.m_genJets.end(); ++it2)
				{
					if ((it2->second->size() > 0)
						&& (ROOT::Math::VectorUtil::DeltaR(it->p4, it2->second->at(0).p4) < R))
						metaData.m_neutrinos[it2->first].push_back(*it);
				}
			}
			else if (it->status() == 1 &&
					 ((std::abs(it->pdgId()) == 130)
					  || (std::abs(it->pdgId()) == 310)
					  || (std::abs(it->pdgId()) == 2112)
					  || (std::abs(it->pdgId()) == 3122)))  //neutral hadrons
			{
				const float pt_threshold = 1.; //GeV
				for (ZJetEventData::GenJetMapIterator it2 = data.m_genJets.begin(); it2 != data.m_genJets.end(); ++it2)
				{
					if ((it2->second->size() > 0)
						&& (ROOT::Math::VectorUtil::DeltaR(it->p4, it2->second->at(0).p4) < R)
						&& (it->p4.Pt() > pt_threshold))
						metaData.m_neutrals5[it2->first].push_back(*it);
					if ((it2->second->size() > 0)
						&& (ROOT::Math::VectorUtil::DeltaR(it->p4, it2->second->at(0).p4) < 0.3)
						&& (it->p4.Pt() > pt_threshold))
						metaData.m_neutrals3[it2->first].push_back(*it);
				}
			}
			else // unexpected particles apart from stable mesons and baryons
			{
				//if (it->status() != 3 && std::abs(it->pdgId()) < 90)
				//	LOG("Unexpected particle " << *it);
			}
		}

		// check for unusual behaviour
		if (metaData.m_genZs.size() < 1)
			LOG("There is no gen Z!");
		if (metaData.m_genPartons.size() < 1)
			LOG("There is no parton!");
		//if (metaData.m_genMuons.size() < 1)
		//	LOG("There are no gen muons!");
		//if (metaData.m_genMuons.size() > 2)
		//	LOG("There are more than 2 gen muons (" << metaData.m_genMuons.size() << ")!")

		return true;
	}

	static std::string Name()
	{
		return "gen_producer";
	}

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
										ZJetMetaData& metaData,
										ZJetPipelineSettings const& globalSettings) const
	{
		// Valid gen Z producer
		if (unlikely(metaData.m_genZs.size() < 1))
		{
			//LOG("No gen Z in the event.")
			metaData.SetValidGenZ(false);
			return true;
		}
		if (metaData.m_genZs.size() > 1)
		{
			//LOG("More than one gen Z in the event.")
			// Could be resolved, but this case is currently not present.
			metaData.SetValidGenZ(false);
			return true;
		}
		if (metaData.m_genZs[0].p4.Pt() < zptmin)
		{
			//LOG("Gen Z pt is very low, no balance calculated.")
			metaData.SetValidGenZ(false);
			return true;
		}

		// check if the Z decays to the muons:
		if (metaData.m_genMuons.size() != 2)
		{
			//LOG("Not exactly two muons in the event, therefore no valid Z.")
			metaData.SetValidGenZ(false);
			return true;
		}
		if (metaData.m_genZs.size() >= 1)
		{
			RMDataLV vec = metaData.m_genMuons[0].p4 + metaData.m_genMuons[1].p4
						   - metaData.m_genZs[0].p4;
			if (vec.Pt() > 1e-3)	// differs more than a MeV
			{
				//LOG("Muons not from Z decay: pt:" << vec.Pt() << ", eta: " << vec.Eta() << ", phi: " << vec.Phi() << ", m: " << vec.M());
				metaData.SetValidGenZ(false);
				return true;
			}
		}
		metaData.SetValidGenZ(true);
		metaData.SetGenZ(metaData.m_genZs[0]);

		//search
		metaData.SetBalanceQuality(threshold);
		metaData.SetValidParton(false);
		if (metaData.m_genPartons.size() < 1)
		{
			LOG("No partons in the event!")
			return true;
		}

		/*		double dphi = -1.;
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
						//	LOG("The best balanced parton is not the leading one!")
						//	LOG("  Best    (" << it->pdgId() << ") Q: " << bQuality << ", pt: " <<it->p4.Pt() << ", dphi: " << dphi << ", R: " << R)
						//	LOG("  Leading (" << metaData.GetRefParton().pdgId() << ") Q: " << metaData.GetRefBalanceQuality() << ", pt: " <<metaData.GetRefParton().p4.Pt())
						//}
						metaData.SetValidParton(true);
						metaData.SetBalanceQuality(bQuality);
						metaData.SetBalancedParton(*it);
						//LOG("Balance (" << it->pdgId() << ") dphi: " << dphi << ", R: " << R << ", Q: " << bQuality)
					}
					if (it == metaData.m_genPartons.begin()
						|| metaData.GetRefLeadingParton().p4.Pt() < it->p4.Pt())
						metaData.SetLeadingParton(*it);
				}

				if (!metaData.GetRefValidParton())
				{
					//LOG("No balance found below threshold=" << metaData.GetRefBalanceQuality() << "!")
					return true;
				}
				//LOG("Best parton is " << metaData.GetRefParton().pdgId() << ": " << metaData.GetRefBalanceQuality())
				//LOG("fill " << met);
				//metaData.SetGenMet(met);
		*/
		return true;
	}

	static std::string Name()
	{
		return "gen_balance_producer";
	}

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
										ZJetMetaData& metaData,
										ZJetPipelineSettings const& globalSettings) const
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
				RMDataLV comb = i->p4 + j->p4;
				dphi = ROOT::Math::VectorUtil::DeltaPhi(comb, metaData.GetRefGenZ().p4);
				dphi = ROOT::Math::VectorUtil::Phi_mpi_pi(dphi - ROOT::Math::Pi());
				R = comb.Pt() / metaData.GetRefGenZ().p4.Pt();
				// decision metric
				bQuality = std::abs(dphi) + 2.0 * std::abs(R - 1.0);

				if (bQuality < metaData.GetRefBalanceQuality())
				{
					metaData.SetValidParton(true);
					metaData.SetBalanceQuality(bQuality);
					if (i->p4.Pt() > j->p4.Pt())
					{
						metaData.SetBalancedParton(*i);
						//LOG("Balance (" << i->pdgId() << ") dphi: " << dphi << ", R: " << R << ", Q: " << bQuality)
					}
					else
					{
						metaData.SetBalancedParton(*j);
						//LOG("Balance (" << i->pdgId() << ") dphi: " << dphi << ", R: " << R << ", Q: " << bQuality)
					}
				}
			}
		return true;
	}

	static std::string Name()
	{
		return "gen_dibalance_producer";
	}
};

/* Produce gen met

   This requires the @see GenProducer and GenBalanceProducer before.
*/
class GenMetProducer: public ZJetGlobalMetaDataProducerBase
{
public:

	GenMetProducer(): ZJetGlobalMetaDataProducerBase(),
		pptmin(0.5), petamax(5.2)
	{}

	virtual bool PopulateGlobalMetaData(ZJetEventData const& data,
										ZJetMetaData& metaData,
										ZJetPipelineSettings const& globalSettings) const
	{
		KDataLV met;
		if (!metaData.HasValidGenZ())
		{
			LOG("No gen Z");
			return true;
		}

		for (auto it = data.m_particles->begin(); it != data.m_particles->end(); ++it)
		{
			// Take only stable final particles
			if (it->status() != 1)
				continue;

			// ignore first 6 particles (because that is what CMSSW does)
			if (it - data.m_particles->begin() < 6)
			{
				continue;
			}

			if (unlikely(it->children != 0))
				LOG("Particle has " << it->children << " children.");

			//LOG ("add " << *it);
			if (abs(it->p4.Eta()) > petamax || it->p4.Pt() < pptmin)
				continue;
			met.p4 = met.p4 + it->p4;
		}

		metaData.SetGenMet(met);
		return true;
	}

	static std::string Name()
	{
		return "gen_met_producer";
	}

private:
	const double pptmin;
	const double petamax;
};

}
