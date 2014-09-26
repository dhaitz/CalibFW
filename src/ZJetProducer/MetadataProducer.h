#pragma once

// from ROOT
#include <Math/VectorUtil.h>
#include "ZJetEventPipeline/Pipeline.h"

namespace Artus
{

typedef GlobalMetaDataProducerBase<ZJetEventData, ZJetMetaData, ZJetPipelineSettings>
ZJetGlobalMetaDataProducerBase;


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
		nmin(50), nmax(2500), verbose(false)
	{}

	virtual bool PopulateGlobalMetaData(ZJetEventData const& data,
										ZJetMetaData& metaData, ZJetPipelineSettings const& globalSettings) const
	{
		// Check number of particles (could be simplified after study)
		if (verbose)
		{
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
		}

		const float R = 0.5; // for DeltaR matching

		// Loop over particles
		for (auto it = data.m_particles->begin(); it != data.m_particles->end(); ++it)
		{
			// Take only stable final particles
			//if (it->status() != 3)
			//	continue;
			//LOG("Particle " << *it);
			// ignore first 6 particles (because that is what CMSSW does)
			if (it - data.m_particles->begin() < 6)
				continue;

			if (verbose && unlikely(it->numberOfDaughters() != 0))
				LOG("Particle has " << it->numberOfDaughters() << " children.");
			//if (std::abs(it->pdgId()) == 13) LOG("P " <<*it);
			// Sort particles in lists in metaData
			if (std::abs(it->pdgId()) == 13 && it->status() == 1)		// stable muon
			{
				metaData.m_genMuons.emplace_back(*it);
			}
			else if (std::abs(it->pdgId()) == 13 && it->status() == 2)		// intermediate muon
			{
				metaData.m_genIntermediateMuons.emplace_back(*it);
			}
			else if (std::abs(it->pdgId()) == 13 && it->status() == 3)		// internal muon
			{
				metaData.m_genInternalMuons.emplace_back(*it);
			}
			else if (std::abs(it->pdgId()) == 11 && it->status() == 1)		// stable muon
			{
				metaData.m_genElectrons.emplace_back(*it);
			}
			else if (std::abs(it->pdgId()) == 11 && it->status() == 2)		// intermediate muon
			{
				metaData.m_genIntermediateElectrons.emplace_back(*it);
			}
			else if (std::abs(it->pdgId()) == 11 && it->status() == 3)		// internal muon
			{
				metaData.m_genInternalElectrons.emplace_back(*it);
			}
			else if (std::abs(it->pdgId()) == 23 && it->status() == 2)	// Z
			{
				metaData.m_genZs.emplace_back(*it);
			}
			else if (std::abs(it->pdgId()) == 22 && it->status() == 1)	// photon
			{
				metaData.m_genPhotons.emplace_back(*it);
			}
			else if (std::abs(it->pdgId()) < 7 || std::abs(it->pdgId()) == 21)	// parton
			{
				metaData.m_genPartons.emplace_back(*it);
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
						metaData.m_neutrinos[it2->first].emplace_back(*it);
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
						metaData.m_neutrals5[it2->first].emplace_back(*it);
					if ((it2->second->size() > 0)
						&& (ROOT::Math::VectorUtil::DeltaR(it->p4, it2->second->at(0).p4) < 0.3)
						&& (it->p4.Pt() > pt_threshold))
						metaData.m_neutrals3[it2->first].emplace_back(*it);
				}
			}
			else if (verbose) // unexpected particles apart from stable mesons and baryons
			{
				if (it->status() != 3 && std::abs(it->pdgId()) < 90)
					LOG("Unexpected particle " << *it);
			}
		}

		// check for unusual behaviour
		if (verbose)
		{
			if (metaData.m_genZs.size() < 1)
				LOG("There is no gen Z!");
			if (metaData.m_genPartons.size() < 1)
				LOG("There is no parton!");
			if (metaData.m_genMuons.size() < 1)
				LOG("There are no gen muons!");
			if (metaData.m_genMuons.size() > 2)
				LOG("There are more than 2 gen muons (" << metaData.m_genMuons.size() << ")!")
			}

		return true;
	}

	static std::string Name()
	{
		return "gen_producer";
	}

private:
	const unsigned short int nmin;
	const unsigned short int nmax;
	bool verbose;
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
