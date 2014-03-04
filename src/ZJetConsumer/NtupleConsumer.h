#pragma once

#include <string>
#include <memory>

#include "GlobalInclude.h"
#include "RootTools/RootIncludes.h"


namespace Artus
{

template <class TEvent, class TMetaData, class TSettings>
class NtupleConsumerBase : public EventConsumerBase<TEvent, TMetaData, TSettings>
{
public:
	typedef EventPipeline<TEvent, TMetaData, TSettings> PipelineTypeForThis;
};

class NtupleConsumer : public NtupleConsumerBase<ZJetEventData, ZJetMetaData, ZJetPipelineSettings>
{
public:
	static std::string GetName()
	{
		return "ntuple";
	}

	void Init(PipelineTypeForThis* pset)
	{
		EventConsumerBase<ZJetEventData, ZJetMetaData, ZJetPipelineSettings>::Init(pset);
		std::string quantities = this->GetPipelineSettings().GetQuantitiesString();
		m_ntuple = new TNtuple("NTuple", "NTuple", quantities.c_str());

		// turn the stringvector into an enumvector

	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
									  ZJetMetaData const& metaData)
	{
		EventConsumerBase< ZJetEventData, ZJetMetaData, ZJetPipelineSettings>::ProcessFilteredEvent(event, metaData);

		std::vector<float> array;
		std::vector<std::string> stringvector = this->GetPipelineSettings().GetQuantities();

		//iterate over string vector and fill the array for each quantitiy
		for (std::vector<std::string>::iterator it = stringvector.begin(); it != stringvector.end(); ++it)
			array.push_back(returnvalue(*it, event, metaData, this->GetPipelineSettings()));

		// add the array to the ntuple
		m_ntuple->Fill(&array[0]);
	}

	virtual void Finish()
	{
		m_ntuple->Write(this->GetPipelineSettings().GetName().c_str());
	}

private:

	TNtuple* m_ntuple;

	virtual float returnvalue(std::string string, ZJetEventData const& event,
							  ZJetMetaData const& metaData, ZJetPipelineSettings const& s) const
	{
		// general quantities
		if (string == "npv")
			return event.m_vertexSummary->nVertices;
		else if (string == "npu")
			return event.m_geneventmetadata->numPUInteractions0;
		else if (string == "nputruth")
		{
			if (s.IsMC())
				return event.m_geneventmetadata->numPUInteractionsTruth;
			return metaData.GetNpuTruth();
		}

		// QG tag
		else if (string == "qglikelihood")
			return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->getTagger("QGlikelihood", event.m_taggermetadata);
		else if (string == "qgmlp")
			return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->getTagger("QGmlp", event.m_taggermetadata);
		// b tags
		else if (string == "trackcountinghigheffbjettag")
			return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->getTagger("trackCountingHighEffBTag", event.m_taggermetadata);
		else if (string == "trackcountinghighpurbjettag")
			return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->getTagger("trackCountingHighPurBTag", event.m_taggermetadata);
		else if (string == "jetprobabilitybjettag")
			return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->getTagger("jetProbabilityBTag", event.m_taggermetadata);
		else if (string == "jetbprobabilitybjettag")
			return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->getTagger("jetBProbabilityBTag", event.m_taggermetadata);
		else if (string == "softelectronbjettag")
			return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->getTagger("softElectronBTag", event.m_taggermetadata);
		else if (string == "softmuonbjettag")
			return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->getTagger("softMuonBTag", event.m_taggermetadata);
		else if (string == "softmuonbyip3dbjettag")
			return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->getTagger("softMuonByIP3dBTag", event.m_taggermetadata);
		else if (string == "softmuonbyptbjettag")
			return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->getTagger("softMuonByPtBTag", event.m_taggermetadata);
		else if (string == "simplesecondaryvertexbjettag")
			return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->getTagger("simpleSecondaryVertexBTag", event.m_taggermetadata);
		else if (string == "combinedsecondaryvertexbjettag")
			return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->getTagger("CombinedSecondaryVertexBJetTags", event.m_taggermetadata);
		else if (string == "combinedsecondaryvertexmvabjettag")
			return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->getTagger("CombinedSecondaryVertexMVABJetTags", event.m_taggermetadata);

		// jet 1 PU
		else if (string == "jet1puJetIDFullTight")
			return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->getpuJetID("puJetIDFullTight", event.m_taggermetadata);
		else if (string == "jet1puJetIDFullMedium")
			return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->getpuJetID("puJetIDFullMedium", event.m_taggermetadata);
		else if (string == "jet1puJetIDFullLoose")
			return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->getpuJetID("puJetIDFullLoose", event.m_taggermetadata);
		else if (string == "jet1puJetIDCutbasedTight")
			return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->getpuJetID("puJetIDCutbasedTight", event.m_taggermetadata);
		else if (string == "jet1puJetIDCutbasedMedium")
			return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->getpuJetID("puJetIDCutbasedMedium", event.m_taggermetadata);
		else if (string == "jet1puJetIDCutbasedLoose")
			return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->getpuJetID("puJetIDCutbasedLoose", event.m_taggermetadata);

		// jet 2 PU
		else if (string == "jet2puJetIDFullTight")
		{
			if (metaData.GetValidJetCount(s, event) > 1)
				return static_cast<KDataPFTaggedJet*>(metaData.GetValidJet(s, event, 1))->getpuJetID("puJetIDFullTight", event.m_taggermetadata);
			else return 0;
		}
		else if (string == "jet2puJetIDFullMedium")
		{
			if (metaData.GetValidJetCount(s, event) > 1)
				return static_cast<KDataPFTaggedJet*>(metaData.GetValidJet(s, event, 1))->getpuJetID("puJetIDFullMedium", event.m_taggermetadata);
			else return 0;
		}
		else if (string == "jet2puJetIDFullLoose")
		{
			if (metaData.GetValidJetCount(s, event) > 1)
				return static_cast<KDataPFTaggedJet*>(metaData.GetValidJet(s, event, 1))->getpuJetID("puJetIDFullLoose", event.m_taggermetadata);
			else return 0;
		}
		else if (string == "jet2puJetIDCutbasedTight")
		{
			if (metaData.GetValidJetCount(s, event) > 1)
				return static_cast<KDataPFTaggedJet*>(metaData.GetValidJet(s, event, 1))->getpuJetID("puJetIDCutbasedTight", event.m_taggermetadata);
			else return 0;
		}
		else if (string == "jet2puJetIDCutbasedMedium")
		{
			if (metaData.GetValidJetCount(s, event) > 1)
				return static_cast<KDataPFTaggedJet*>(metaData.GetValidJet(s, event, 1))->getpuJetID("puJetIDCutbasedMedium", event.m_taggermetadata);
			else return 0;
		}
		else if (string == "jet2puJetIDCutbasedLoose")
		{
			if (metaData.GetValidJetCount(s, event) > 1)
				return static_cast<KDataPFTaggedJet*>(metaData.GetValidJet(s, event, 1))->getpuJetID("puJetIDCutbasedLoose", event.m_taggermetadata);
			else return 0;
		}
		else if (string == "rho")
			return event.m_jetArea->median;
		else if (string == "run")
			return event.m_eventmetadata->nRun;
		else if (string == "weight")
			return metaData.GetWeight();
		else if (string == "eff")
			return metaData.GetEfficiency();
		// Z
		else if (string == "zpt")
			return metaData.GetRefZ().p4.Pt();
		else if (string == "zeta")
			return metaData.GetRefZ().p4.Eta();
		else if (string == "zphi")
			return metaData.GetRefZ().p4.Phi();
		else if (string == "zy")
			return metaData.GetRefZ().p4.Rapidity();
		else if (string == "zmass")
			return metaData.GetRefZ().p4.mass();

		// muons
		else if (string == "mupluspt")
		{
			for (KDataMuons::const_iterator it = metaData.m_listValidMuons.begin();
				 it != metaData.m_listValidMuons.end(); it ++)
			{
				if (it->charge == 1) return it->p4.Pt();
			}
		}
		else if (string == "mupluseta")
		{
			for (KDataMuons::const_iterator it = metaData.m_listValidMuons.begin();
				 it != metaData.m_listValidMuons.end(); it ++)
			{
				if (it->charge == 1) return it->p4.Eta();
			}
		}
		else if (string == "muplusphi")
		{
			for (KDataMuons::const_iterator it = metaData.m_listValidMuons.begin();
				 it != metaData.m_listValidMuons.end(); it ++)
			{
				if (it->charge == 1) return it->p4.Phi();
			}
		}
		else if (string == "muminuspt")
		{
			for (KDataMuons::const_iterator it = metaData.m_listValidMuons.begin();
				 it != metaData.m_listValidMuons.end(); it ++)
			{
				if (it->charge == -1) return it->p4.Pt();
			}
		}
		else if (string == "muminuseta")
		{
			for (KDataMuons::const_iterator it = metaData.m_listValidMuons.begin();
				 it != metaData.m_listValidMuons.end(); it ++)
			{
				if (it->charge == -1) return it->p4.Eta();
			}
		}
		else if (string == "muminusphi")
		{
			for (KDataMuons::const_iterator it = metaData.m_listValidMuons.begin();
				 it != metaData.m_listValidMuons.end(); it ++)
			{
				if (it->charge == -1) return it->p4.Phi();
			}
		}
		else if (string == "muplusiso")
		{
			for (KDataMuons::const_iterator it = metaData.m_listValidMuons.begin();
				 it != metaData.m_listValidMuons.end(); it ++)
			{
				if (it->charge == +1) return it->trackIso03;
			}
		}
		else if (string == "muminusiso")
		{
			for (KDataMuons::const_iterator it = metaData.m_listValidMuons.begin();
				 it != metaData.m_listValidMuons.end(); it ++)
			{
				if (it->charge == -1) return it->trackIso03;
			}
		}
		else if (string == "mu1pt")
		{
			KDataMuon  muon;
			for (KDataMuons::const_iterator it = metaData.m_listValidMuons.begin();
				 it != metaData.m_listValidMuons.end(); it ++)
			{
				if ((it == metaData.m_listValidMuons.begin()) || (it->p4.Pt() > muon.p4.Pt()))
					muon = *it;
			}
			return muon.p4.Pt();
		}
		else if (string == "mu1phi")
		{
			KDataMuon  muon;
			for (KDataMuons::const_iterator it = metaData.m_listValidMuons.begin();
				 it != metaData.m_listValidMuons.end(); it ++)
			{
				if ((it == metaData.m_listValidMuons.begin()) || (it->p4.Pt() > muon.p4.Pt()))
					muon = *it;
			}
			return muon.p4.Phi();
		}
		else if (string == "mu1eta")
		{
			KDataMuon  muon;
			for (KDataMuons::const_iterator it = metaData.m_listValidMuons.begin();
				 it != metaData.m_listValidMuons.end(); it ++)
			{
				if ((it == metaData.m_listValidMuons.begin()) || (it->p4.Pt() > muon.p4.Pt()))
					muon = *it;
			}
			return muon.p4.Eta();
		}
		else if (string == "mu2pt")
		{
			KDataMuon  muon;
			for (KDataMuons::const_iterator it = metaData.m_listValidMuons.begin();
				 it != metaData.m_listValidMuons.end(); it ++)
			{
				if ((it == metaData.m_listValidMuons.begin()) || (it->p4.Pt() < muon.p4.Pt()))
					muon = *it;
			}
			return muon.p4.Pt();
		}
		else if (string == "mu2phi")
		{
			KDataMuon  muon;
			for (KDataMuons::const_iterator it = metaData.m_listValidMuons.begin();
				 it != metaData.m_listValidMuons.end(); it ++)
			{
				if ((it == metaData.m_listValidMuons.begin()) || (it->p4.Pt() < muon.p4.Pt()))
					muon = *it;
			}
			return muon.p4.Phi();
		}
		else if (string == "mu2eta")
		{
			KDataMuon  muon;
			for (KDataMuons::const_iterator it = metaData.m_listValidMuons.begin();
				 it != metaData.m_listValidMuons.end(); it ++)
			{
				if ((it == metaData.m_listValidMuons.begin()) || (it->p4.Pt() < muon.p4.Pt()))
					muon = *it;
			}
			return muon.p4.Eta();
		}
		else if (string == "genmupluspt")
		{
			for (auto it = metaData.m_genMuons.begin(); it != metaData.m_genMuons.end(); it++)
				if (it->charge() > 0) return it->p4.Pt();
			return -999;
		}
		else if (string == "genmupluseta")
		{
			for (auto it = metaData.m_genMuons.begin(); it != metaData.m_genMuons.end(); it++)
				if (it->charge() > 0) return it->p4.Eta();
			return -999;
		}
		else if (string == "genmuplusphi")
		{
			for (auto it = metaData.m_genMuons.begin(); it != metaData.m_genMuons.end(); it++)
				if (it->charge() > 0) return it->p4.Phi();
			return -999;
		}
		else if (string == "genmuminuspt")
		{
			for (auto it = metaData.m_genMuons.begin(); it != metaData.m_genMuons.end(); it++)
				if (it->charge() < 0) return it->p4.Pt();
			return -999;
		}
		else if (string == "genmuminuseta")
		{
			for (auto it = metaData.m_genMuons.begin(); it != metaData.m_genMuons.end(); it++)
				if (it->charge() < 0) return it->p4.Eta();
			return -999;
		}
		else if (string == "genmuminusphi")
		{
			for (auto it = metaData.m_genMuons.begin(); it != metaData.m_genMuons.end(); it++)
				if (it->charge() < 0) return it->p4.Phi();
			return -999;
		}
		else if (string == "nmuons")
			return metaData.m_listValidMuons.size();
		else if (string == "ngenmuons")
			return metaData.m_genMuons.size();

		// jets
		else if (string == "njets")
			return metaData.GetValidJetCount(s, event);
		else if (string == "njetsinv")
			return metaData.m_listInvalidJets["AK5PFJetsCHS"].size();
		else if (string == "ngenphotons")
			return metaData.m_genPhotons.size();
		else if (string == "nzs")
			return metaData.m_genZs.size();
		else if (string == "ninternalmuons")
			return metaData.m_genInternalMuons.size();
		else if (string == "nintermediatemuons")
			return metaData.m_genIntermediateMuons.size();
		else if (string == "closestphotondr")
		{
			double dR = 999.;
			double d = dR;
			for (auto mu = metaData.m_genMuons.begin(); mu != metaData.m_genMuons.end(); mu++)
				for (auto ph = metaData.m_genPhotons.begin(); ph != metaData.m_genPhotons.end(); ph++)
				{
					d = ROOT::Math::VectorUtil::DeltaR(mu->p4, ph->p4);
					if (d < dR && ph->p4.Pt() > 1)
						dR = d;
				}
			return dR;
		}
		else if (string == "ngenphotonsclose")
		{
			const double dR = 0.3;
			int n = 0;
			for (auto mu = metaData.m_genMuons.begin(); mu != metaData.m_genMuons.end(); mu++)
				for (auto ph = metaData.m_genPhotons.begin(); ph != metaData.m_genPhotons.end(); ph++)
					if (ROOT::Math::VectorUtil::DeltaR(mu->p4, ph->p4) < dR)
						n++;
			return n;
		}
		else if (string == "ptgenphotonsclose")
		{
			const double dR = 0.3;
			double pt = 0;
			for (auto mu = metaData.m_genMuons.begin(); mu != metaData.m_genMuons.end(); mu++)
				for (auto ph = metaData.m_genPhotons.begin(); ph != metaData.m_genPhotons.end(); ph++)
					if (ROOT::Math::VectorUtil::DeltaR(mu->p4, ph->p4) < dR)
						pt += ph->p4.Pt();
			return pt;
		}
		else if (string == "ptgenphotonsfar")
		{
			const double dR = 0.3;
			double pt = 0;
			for (auto mu = metaData.m_genMuons.begin(); mu != metaData.m_genMuons.end(); mu++)
			{
				// look at a cone perpendicular to the muon
				KGenParticle nomu = *mu;
				nomu.p4.SetPhi(nomu.p4.Phi() + ROOT::Math::Pi() / 2);
				for (auto ph = metaData.m_genPhotons.begin(); ph != metaData.m_genPhotons.end(); ph++)
					if (ROOT::Math::VectorUtil::DeltaR(nomu.p4, ph->p4) < dR)
						pt += ph->p4.Pt();
			}
			return pt;
		}
		else if (string == "ptdiff13")
		{
			if (metaData.m_genMuons.size() < 2 || metaData.m_genInternalMuons.size() < 2)
				return -999;
			double diff = 0;
			diff = metaData.m_genMuons[0].p4.Pt()
				   + metaData.m_genMuons[1].p4.Pt()
				   - metaData.m_genInternalMuons[0].p4.Pt()
				   - metaData.m_genInternalMuons[1].p4.Pt();
			return diff;
		}
		else if (string == "ptdiff12")
		{
			if (metaData.m_genMuons.size() < 2 || metaData.m_genIntermediateMuons.size() < 2)
				return -999;
			double diff = 0;
			diff = metaData.m_genMuons[0].p4.Pt()
				   + metaData.m_genMuons[1].p4.Pt()
				   - metaData.m_genIntermediateMuons[0].p4.Pt()
				   - metaData.m_genIntermediateMuons[1].p4.Pt();
			return diff;
		}
		else if (string == "ptdiff23")
		{
			if (metaData.m_genIntermediateMuons.size() < 2 || metaData.m_genInternalMuons.size() < 2)
				return -999;
			double diff = 0;
			diff = metaData.m_genMuons[0].p4.Pt()
				   + metaData.m_genMuons[1].p4.Pt()
				   - metaData.m_genIntermediateMuons[0].p4.Pt()
				   - metaData.m_genIntermediateMuons[1].p4.Pt();
			return diff;
		}
		else if (string == "genzpt")
		{
			if (metaData.m_genZs.size() < 1)
				return -1;
			return metaData.m_genZs[0].p4.Pt();
		}

		// electrons
		else if (string == "nelectrons")
			return metaData.GetValidElectrons().size();

		else if (string == "emass")
			return metaData.leadinge.p4.mass();
		else if (string == "ept")
			return metaData.leadinge.p4.Pt();
		else if (string == "eeta")
			return metaData.leadinge.p4.Eta();
		else if (string == "eminusmass")
			return metaData.leadingeminus.p4.mass();
		else if (string == "eminuspt")
			return metaData.leadingeminus.p4.Pt();
		else if (string == "eminuseta")
			return metaData.leadingeminus.p4.Eta();
		else if (string == "eplusmass")
			return metaData.leadingeplus.p4.mass();
		else if (string == "epluspt")
			return metaData.leadingeplus.p4.Pt();
		else if (string == "epluseta")
			return metaData.leadingeplus.p4.Eta();



		else if (string == "eminusiso")
			return metaData.leadingeminus.trackIso03;
		else if (string == "eplusiso")
			return metaData.leadingeplus.trackIso03;

		else if (string == "eminusecaliso03")
			return metaData.leadingeminus.ecalIso03;
		else if (string == "eplusecaliso03")
			return metaData.leadingeplus.ecalIso03;

		else if (string == "eminusecaliso04")
			return metaData.leadingeminus.ecalIso04;
		else if (string == "eplusecaliso04")
			return metaData.leadingeplus.ecalIso04;

		else if (string == "eminusid")
			return metaData.leadingeminus.idMvaNonTrigV0;
		else if (string == "eplusid")
			return metaData.leadingeplus.idMvaNonTrigV0;

		else if (string == "eminustrigid")
			return metaData.leadingeminus.idMvaTrigV0;
		else if (string == "eplustrigid")
			return metaData.leadingeplus.idMvaTrigV0;


		// leading jet
		else if (string == "jet1pt")
			return metaData.GetValidPrimaryJet(s, event)->p4.Pt();
		else if (string == "jet1eta")
			return metaData.GetValidPrimaryJet(s, event)->p4.Eta();
		else if (string == "jet1phi")
			return metaData.GetValidPrimaryJet(s, event)->p4.Phi();

		// leading jet composition
		else if (string == "jet1photonfraction")
			return static_cast<KDataPFJet*>(metaData.GetValidPrimaryJet(s, event))->photonFraction;
		else if (string == "jet1chargedemfraction")
			return static_cast<KDataPFJet*>(metaData.GetValidPrimaryJet(s, event))->chargedEMFraction;
		else if (string == "jet1chargedhadfraction")
			return static_cast<KDataPFJet*>(metaData.GetValidPrimaryJet(s, event))->chargedHadFraction;
		else if (string == "jet1neutralhadfraction")
			return static_cast<KDataPFJet*>(metaData.GetValidPrimaryJet(s, event))->neutralHadFraction;
		else if (string == "jet1muonfraction")
			return static_cast<KDataPFJet*>(metaData.GetValidPrimaryJet(s, event))->muonFraction;
		else if (string == "jet1HFhadfraction")
			return static_cast<KDataPFJet*>(metaData.GetValidPrimaryJet(s, event))->HFHadFraction;
		else if (string == "jet1HFemfraction")
			return static_cast<KDataPFJet*>(metaData.GetValidPrimaryJet(s, event))->HFEMFraction;

		// second jet
		else if (string == "jet2pt")
		{
			if (metaData.GetValidJetCount(s, event) > 1)
				return metaData.GetValidJet(s, event, 1)->p4.Pt();
			else
				return 0;
		}
		else if (string == "jet2phi")
		{
			if (metaData.GetValidJetCount(s, event) > 1)
				return metaData.GetValidJet(s, event, 1)->p4.Phi();
			else
				return 0;
		}
		else if (string == "jet2eta")
		{
			if (metaData.GetValidJetCount(s, event) > 1)
				return metaData.GetValidJet(s, event, 1)->p4.Eta();
			else
				return 0;
		}

		// MET & sumEt
		else if (string == "METpt")
			return metaData.GetMet(event, s)->p4.Pt();
		else if (string == "METphi")
			return metaData.GetMet(event, s)->p4.Phi();
		else if (string == "sumEt")
			return metaData.GetMet(event, s)->sumEt;
		else if (string == "rawMETpt")
			return event.GetMet(s)->p4.Pt();
		else if (string == "rawMETphi")
			return event.GetMet(s)->p4.Phi();

		else if (string == "uept")
			return metaData.GetUE(event, s)->p4.Pt();
		else if (string == "uephi")
			return metaData.GetUE(event, s)->p4.Phi();
		else if (string == "ueeta")
			return metaData.GetUE(event, s)->p4.Eta();
		else if (string == "mpf")
			return metaData.GetMPF(metaData.GetMet(event, s));
		else if (string == "rawmpf")
			return metaData.GetMPF(event.GetMet(s));
		else if (string == "otherjetspt")
		{
			if (metaData.GetValidJetCount(s, event) < 2)
				return 0;
			else
				return (-(metaData.GetRefZ().p4
						  + metaData.GetValidPrimaryJet(s, event)->p4
						  + metaData.GetMet(event, s)->p4
						  + metaData.GetValidJet(s, event, 1)->p4
						  + metaData.GetUE(event, s)->p4
						 )).Pt();
		}
		else if (string == "otherjetsphi")
		{
			if (metaData.GetValidJetCount(s, event) < 2)
				return 0;
			else
				return (-(metaData.GetRefZ().p4
						  + metaData.GetValidPrimaryJet(s, event)->p4
						  + metaData.GetMet(event, s)->p4
						  + metaData.GetValidJet(s, event, 1)->p4
						  + metaData.GetUE(event, s)->p4
						 )).Phi();
		}
		else if (string == "otherjetseta")
		{
			if (metaData.GetValidJetCount(s, event) < 2)
				return 0;
			else
				return (-(metaData.GetRefZ().p4
						  + metaData.GetValidPrimaryJet(s, event)->p4
						  + metaData.GetMet(event, s)->p4
						  + metaData.GetValidJet(s, event, 1)->p4
						  + metaData.GetUE(event, s)->p4
						 )).Eta();
		}
		//gen jets
		else if (string == "genjet1pt")
		{
			std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));

			if (metaData.GetValidJetCount(s, event, genName) == 0)
				return false;

			return metaData.GetValidJet(s, event, 0, genName)->p4.Pt();
		}
		else if (string == "genjet1ptneutrinos")
		{
			std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));

			if (metaData.GetValidJetCount(s, event, genName) == 0)
				return false;

			KDataLV v = * metaData.GetValidJet(s, event, 0, genName);
			if (metaData.m_neutrinos[genName].size() > 0)
				for (auto it = metaData.m_neutrinos[genName].begin(); it != metaData.m_neutrinos[genName].end(); ++it)
					v.p4 += it->p4;
			return v.p4.Pt();
		}
		else if (string == "genjet1eta")
		{
			std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));

			if (metaData.GetValidJetCount(s, event, genName) == 0)
				return false;

			return metaData.GetValidJet(s, event, 0, genName)->p4.Eta();
		}
		else if (string == "genjet1phi")
		{
			std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));

			if (metaData.GetValidJetCount(s, event, genName) == 0)
				return false;

			return metaData.GetValidJet(s, event, 0, genName)->p4.Phi();
		}
		else if (string == "matchedgenjet1pt")
		{
			std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));

			if (0 >= metaData.GetValidJetCount(s, event, genName)
				|| 0 >= metaData.GetValidJetCount(s, event))
				return false;

			if (metaData.m_matchingResults.find(genName) == metaData.m_matchingResults.end())
				return false;
			std::vector<int> const& matchList = metaData.m_matchingResults.at(genName);

			if (unlikely(0 >= matchList.size()))
				return false;

			int iMatchedGen = matchList.at(0);
			if (iMatchedGen <= -1)
				return false;

			if (iMatchedGen >= int(metaData.GetValidJetCount(s, event, genName)))
			{
				LOG_FATAL("Reco to gen matching: No reference gen jet found! "
						  << iMatchedGen << " >= " << metaData.GetValidJetCount(s, event, genName));
				return false;
			}
			return metaData.GetValidJet(s, event, iMatchedGen, genName)->p4.Pt();
		}

		else if (string == "genjet2pt")
		{
			std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));

			if (metaData.GetValidJetCount(s, event, genName) < 2)
				return false;

			return metaData.GetValidJet(s, event, 1, genName)->p4.Pt();
		}
		else if (string == "genzpt")
			return metaData.GetRefGenZ().p4.Pt();
		else if (string == "genmpf")
			return metaData.GetGenMPF(metaData.GetPtGenMet());
		else if (string == "algoflavour")
			return metaData.GetAlgoFlavour(s);
		else if (string == "physflavour")
			return metaData.GetPhysFlavour(s);
		else if (string == "algopt")
			return metaData.GetAlgoPt(s);
		else if (string == "physpt")
			return metaData.GetPhysPt(s);
		else if (string == "eventnr")
			return event.m_eventmetadata->nEvent;
		else if (string == "lumisec")
			return event.m_eventmetadata->nLumi;
		else if (string == "jet1ptneutrinos")
		{
			KDataLV v = * metaData.GetValidPrimaryJet(s, event);
			std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));
			if (metaData.m_neutrinos[genName].size() > 0)
				for (auto it = metaData.m_neutrinos[genName].begin(); it != metaData.m_neutrinos[genName].end(); ++it)
					v.p4 += it->p4;
			return v.p4.Pt();
		}
		else if (string == "mpfneutrinos")
		{
			KDataPFMET met = * metaData.GetMet(event, s);
			std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));
			if (metaData.m_neutrinos[genName].size() > 0)
				for (auto it = metaData.m_neutrinos[genName].begin(); it != metaData.m_neutrinos[genName].end(); ++it)
					met.p4 -= it->p4;
			met.p4.SetEta(0);
			return metaData.GetMPF(&met);
		}
		else if (string == "neutralpt3")
		{
			KDataLV v;
			std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));
			if (metaData.m_neutrals3[genName].size() > 0)
				for (auto it = metaData.m_neutrals3[genName].begin(); it != metaData.m_neutrals3[genName].end(); ++it)
					v.p4 += it->p4;
			return v.p4.Pt();
		}
		else if (string == "neutralpt5")
		{
			KDataLV v;
			std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));
			if (metaData.m_neutrals5[genName].size() > 0)
				for (auto it = metaData.m_neutrals5[genName].begin(); it != metaData.m_neutrals5[genName].end(); ++it)
					v.p4 += it->p4;
			return v.p4.Pt();
		}
		else if (string == "unc")
			return metaData.leadingjetuncertainty[s.GetJetAlgorithm()];
		else
			LOG_FATAL("NtupleConsumer: Quantity (" << string << ") not available!");

		LOG_FATAL("None found");
		assert(false);
		return -999;
	};

};

}
