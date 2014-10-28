#pragma once

#include <string>
#include <memory>

#include "GlobalInclude.h"
#include "RootTools/RootIncludes.h"


namespace Artus
{

template <class TEvent, class TProduct, class TSettings>
class NtupleConsumerBase : public EventConsumerBase<TEvent, TProduct, TSettings>
{
public:
	typedef EventPipeline<TEvent, TProduct, TSettings> PipelineTypeForThis;
};

class NtupleConsumer : public NtupleConsumerBase<ZJetEventData, ZJetProduct, ZJetPipelineSettings>
{
public:
	static std::string GetName()
	{
		return "ntuple";
	}

	void Init(PipelineTypeForThis* pset)
	{
		EventConsumerBase<ZJetEventData, ZJetProduct, ZJetPipelineSettings>::Init(pset);
		std::string quantities = this->GetPipelineSettings().GetQuantitiesString();
		m_ntuple = new TNtuple("NTuple", "NTuple", quantities.c_str());

		m_quantities = this->GetPipelineSettings().GetQuantities();

		fillEnumVector(m_quantities);

	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
									  ZJetProduct const& product)
	{
		EventConsumerBase< ZJetEventData, ZJetProduct, ZJetPipelineSettings>::ProcessFilteredEvent(event, product);

		std::vector<float> array;
		//iterate over string vector and fill the array for each quantitiy
		for (const auto & e : m_enumvector)
			array.push_back(returnvalue(e, event, product, this->GetPipelineSettings()));

		// add the array to the ntuple
		m_ntuple->Fill(&array[0]);
	}

	virtual void Finish()
	{
		m_ntuple->Write(this->GetPipelineSettings().GetName().c_str());
	}

private:

	TNtuple* m_ntuple;
	std::vector<std::string> m_quantities;
	int m_size;

	enum E
	{
		npv,
		npu,
		nputruth,
		qglikelihood,
		qgmlp,
		trackcountinghigheffbjettag,
		trackcountinghighpurbjettag,
		jetprobabilitybjettag,
		jetbprobabilitybjettag,
		softelectronbjettag,
		softmuonbjettag,
		softmuonbyip3dbjettag,
		softmuonbyptbjettag,
		simplesecondaryvertexbjettag,
		combinedsecondaryvertexbjettag,
		combinedsecondaryvertexmvabjettag,
		jet1puJetIDFullTight,
		jet1puJetIDFullMedium,
		jet1puJetIDFullLoose,
		jet1puJetIDCutbasedTight,
		jet1puJetIDCutbasedMedium,
		jet1puJetIDCutbasedLoose,
		jet2puJetIDFullTight,
		jet2puJetIDFullMedium,
		jet2puJetIDFullLoose,
		jet2puJetIDCutbasedTight,
		jet2puJetIDCutbasedMedium,
		jet2puJetIDCutbasedLoose,
		rho,
		run,
		weight,
		eff,
		zpt,
		zeta,
		zphi,
		zy,
		zmass,
		mupluspt,
		mupluseta,
		muplusphi,
		muminuspt,
		muminuseta,
		muminusphi,
		muplusiso,
		muminusiso,
		mu1pt,
		mu1phi,
		mu1eta,
		mu2pt,
		mu2phi,
		mu2eta,
		genmupluspt,
		genmupluseta,
		genmuplusphi,
		genmuminuspt,
		genmuminuseta,
		genmuminusphi,
		nmuons,
		ngenmuons,
		genepluspt,
		genepluseta,
		geneplusphi,
		geneminuspt,
		geneminuseta,
		geneminusphi,
		ngenelectrons,
		ngeninternalelectrons,
		ngenintermediateelectrons,
		njets,
		njets30,
		njets30barrel,
		njetsinv,
		ngenphotons,
		nzs,
		ninternalmuons,
		nintermediatemuons,
		closestphotondr,
		ngenphotonsclose,
		ptgenphotonsclose,
		ptgenphotonsfar,
		ptdiff13,
		ptdiff12,
		ptdiff23,
		genzy,
		genzmass,
		deltaRzgenz,
		deltaReplusgeneplus,
		deltaReminusgeneminus,
		lhezpt,
		lhezeta,
		lhezy,
		lhezphi,
		lhezmass,
		nlhemuons,
		nlheelectrons,
		nlhetaus,
		nelectrons,
		emass,
		ept,
		eeta,
		eminusmass,
		eminuspt,
		eminuseta,
		eminusphi,
		eplusmass,
		epluspt,
		epluseta,
		eplusphi,
		eminusiso,
		eplusiso,
		eminusecaliso03,
		eplusecaliso03,
		eminusecaliso04,
		eplusecaliso04,
		eminusid,
		eplusid,
		eminustrigid,
		eplustrigid,
		jet1pt,
		jet1eta,
		jet1y,
		jet1phi,
		jet1photonfraction,
		jet1chargedemfraction,
		jet1chargedhadfraction,
		jet1neutralhadfraction,
		jet1muonfraction,
		jet1HFhadfraction,
		jet1HFemfraction,
		jet2pt,
		jet2phi,
		jet2eta,
		METpt,
		METphi,
		sumEt,
		rawMETpt,
		rawMETphi,
		uept,
		uephi,
		ueeta,
		mpf,
		rawmpf,
		otherjetspt,
		otherjetsphi,
		otherjetseta,
		genjet1pt,
		genjet1ptneutrinos,
		genjet1eta,
		genjet1phi,
		matchedgenjet1pt,
		matchedgenjet2pt,
		genjet2pt,
		genzpt,
		genmpf,
		algoflavour,
		physflavour,
		algopt,
		physpt,
		eventnr,
		lumisec,
		jet1ptneutrinos,
		mpfneutrinos,
		neutralpt3,
		neutralpt5,
		unc,
		hlt,
		sf,
		sfplus,
		sfminus,
		eidveto,
		eplusidloose,
		eplusidmedium,
		eplusidtight,
		eplusidveto,
		eminusidloose,
		eminusidmedium,
		eminusidtight,
		eminusidveto,
		algol5pt,
		physl5pt,
		mpfalgo,
		mpfphys,
		mpfneutrinosalgo,
		mpfneutrinosphys,
		jet1ptneutrinosalgo,
		jet1ptneutrinosphys
	};
	std::vector<E> m_enumvector;


	float returnvalue(E e, ZJetEventData const& event,
					  ZJetProduct const& product, ZJetPipelineSettings const& s) const
	{
		// general quantities
		if (e == E::npv)
			return event.m_vertexSummary->nVertices;
		else if (e == E::npu)
			return event.m_geneventproduct->numPUInteractions0;
		else if (e == E::nputruth)
		{
			if (s.IsMC())
				return event.m_geneventproduct->numPUInteractionsTruth;
			return product.GetNpuTruth();
		}

		// QG tag
		else if (e == E::qglikelihood)
			return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getTagger("QGlikelihood", event.m_taggerproduct);
		else if (e == E::qgmlp)
			return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getTagger("QGmlp", event.m_taggerproduct);
		// b tags
		else if (e == E::trackcountinghigheffbjettag)
			return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getTagger("trackCountingHighEffBTag", event.m_taggerproduct);
		else if (e == E::trackcountinghighpurbjettag)
			return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getTagger("trackCountingHighPurBTag", event.m_taggerproduct);
		else if (e == E::jetprobabilitybjettag)
			return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getTagger("jetProbabilityBTag", event.m_taggerproduct);
		else if (e == E::jetbprobabilitybjettag)
			return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getTagger("jetBProbabilityBTag", event.m_taggerproduct);
		else if (e == E::softelectronbjettag)
			return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getTagger("softElectronBTag", event.m_taggerproduct);
		else if (e == E::softmuonbjettag)
			return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getTagger("softMuonBTag", event.m_taggerproduct);
		else if (e == E::softmuonbyip3dbjettag)
			return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getTagger("softMuonByIP3dBTag", event.m_taggerproduct);
		else if (e == E::softmuonbyptbjettag)
			return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getTagger("softMuonByPtBTag", event.m_taggerproduct);
		else if (e == E::simplesecondaryvertexbjettag)
			return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getTagger("simpleSecondaryVertexBTag", event.m_taggerproduct);
		else if (e == E::combinedsecondaryvertexbjettag)
			return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getTagger("CombinedSecondaryVertexBJetTags", event.m_taggerproduct);
		else if (e == E::combinedsecondaryvertexmvabjettag)
			return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getTagger("CombinedSecondaryVertexMVABJetTags", event.m_taggerproduct);

		// jet 1 PU
		else if (e == E::jet1puJetIDFullTight)
			return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getpuJetID("puJetIDFullTight", event.m_taggerproduct);
		else if (e == E::jet1puJetIDFullMedium)
			return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getpuJetID("puJetIDFullMedium", event.m_taggerproduct);
		else if (e == E::jet1puJetIDFullLoose)
			return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getpuJetID("puJetIDFullLoose", event.m_taggerproduct);
		else if (e == E::jet1puJetIDCutbasedTight)
			return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getpuJetID("puJetIDCutbasedTight", event.m_taggerproduct);
		else if (e == E::jet1puJetIDCutbasedMedium)
			return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getpuJetID("puJetIDCutbasedMedium", event.m_taggerproduct);
		else if (e == E::jet1puJetIDCutbasedLoose)
			return static_cast<KDataPFTaggedJet*>(product.GetValidPrimaryJet(s, event))->getpuJetID("puJetIDCutbasedLoose", event.m_taggerproduct);

		// jet 2 PU
		else if (e == E::jet2puJetIDFullTight)
		{
			if (product.GetValidJetCount(s, event) > 1)
				return static_cast<KDataPFTaggedJet*>(product.GetValidJet(s, event, 1))->getpuJetID("puJetIDFullTight", event.m_taggerproduct);
			else return 0;
		}
		else if (e == E::jet2puJetIDFullMedium)
		{
			if (product.GetValidJetCount(s, event) > 1)
				return static_cast<KDataPFTaggedJet*>(product.GetValidJet(s, event, 1))->getpuJetID("puJetIDFullMedium", event.m_taggerproduct);
			else return 0;
		}
		else if (e == E::jet2puJetIDFullLoose)
		{
			if (product.GetValidJetCount(s, event) > 1)
				return static_cast<KDataPFTaggedJet*>(product.GetValidJet(s, event, 1))->getpuJetID("puJetIDFullLoose", event.m_taggerproduct);
			else return 0;
		}
		else if (e == E::jet2puJetIDCutbasedTight)
		{
			if (product.GetValidJetCount(s, event) > 1)
				return static_cast<KDataPFTaggedJet*>(product.GetValidJet(s, event, 1))->getpuJetID("puJetIDCutbasedTight", event.m_taggerproduct);
			else return 0;
		}
		else if (e == E::jet2puJetIDCutbasedMedium)
		{
			if (product.GetValidJetCount(s, event) > 1)
				return static_cast<KDataPFTaggedJet*>(product.GetValidJet(s, event, 1))->getpuJetID("puJetIDCutbasedMedium", event.m_taggerproduct);
			else return 0;
		}
		else if (e == E::jet2puJetIDCutbasedLoose)
		{
			if (product.GetValidJetCount(s, event) > 1)
				return static_cast<KDataPFTaggedJet*>(product.GetValidJet(s, event, 1))->getpuJetID("puJetIDCutbasedLoose", event.m_taggerproduct);
			else return 0;
		}
		else if (e == E::rho)
			return event.m_jetArea->median;
		else if (e == E::run)
			return event.m_eventproduct->nRun;
		else if (e == E::weight)
			return product.GetWeight();
		else if (e == E::eff)
			return product.GetEfficiency();
		// Z
		else if (e == E::zpt)
			return product.GetRefZ().p4.Pt();
		else if (e == E::zeta)
			return product.GetRefZ().p4.Eta();
		else if (e == E::zphi)
			return product.GetRefZ().p4.Phi();
		else if (e == E::zy)
			return product.GetRefZ().p4.Rapidity();
		else if (e == E::zmass)
			return product.GetRefZ().p4.mass();

		// muons
		else if (e == E::mupluspt)
		{
			for (KDataMuons::const_iterator it = product.m_listValidMuons.begin();
				 it != product.m_listValidMuons.end(); it ++)
			{
				if (it->charge == 1) return it->p4.Pt();
			}
		}
		else if (e == E::mupluseta)
		{
			for (KDataMuons::const_iterator it = product.m_listValidMuons.begin();
				 it != product.m_listValidMuons.end(); it ++)
			{
				if (it->charge == 1) return it->p4.Eta();
			}
		}
		else if (e == E::muplusphi)
		{
			for (KDataMuons::const_iterator it = product.m_listValidMuons.begin();
				 it != product.m_listValidMuons.end(); it ++)
			{
				if (it->charge == 1) return it->p4.Phi();
			}
		}
		else if (e == E::muminuspt)
		{
			for (KDataMuons::const_iterator it = product.m_listValidMuons.begin();
				 it != product.m_listValidMuons.end(); it ++)
			{
				if (it->charge == -1) return it->p4.Pt();
			}
		}
		else if (e == E::muminuseta)
		{
			for (KDataMuons::const_iterator it = product.m_listValidMuons.begin();
				 it != product.m_listValidMuons.end(); it ++)
			{
				if (it->charge == -1) return it->p4.Eta();
			}
		}
		else if (e == E::muminusphi)
		{
			for (KDataMuons::const_iterator it = product.m_listValidMuons.begin();
				 it != product.m_listValidMuons.end(); it ++)
			{
				if (it->charge == -1) return it->p4.Phi();
			}
		}
		else if (e == E::muplusiso)
		{
			for (KDataMuons::const_iterator it = product.m_listValidMuons.begin();
				 it != product.m_listValidMuons.end(); it ++)
			{
				if (it->charge == +1) return it->trackIso03;
			}
		}
		else if (e == E::muminusiso)
		{
			for (KDataMuons::const_iterator it = product.m_listValidMuons.begin();
				 it != product.m_listValidMuons.end(); it ++)
			{
				if (it->charge == -1) return it->trackIso03;
			}
		}
		else if (e == E::mu1pt)
		{
			KDataMuon muon;
			for (KDataMuons::const_iterator it = product.m_listValidMuons.begin();
				 it != product.m_listValidMuons.end(); it ++)
			{
				if ((it == product.m_listValidMuons.begin()) || (it->p4.Pt() > muon.p4.Pt()))
					muon = *it;
			}
			return muon.p4.Pt();
		}
		else if (e == E::mu1phi)
		{
			KDataMuon muon;
			for (KDataMuons::const_iterator it = product.m_listValidMuons.begin();
				 it != product.m_listValidMuons.end(); it ++)
			{
				if ((it == product.m_listValidMuons.begin()) || (it->p4.Pt() > muon.p4.Pt()))
					muon = *it;
			}
			return muon.p4.Phi();
		}
		else if (e == E::mu1eta)
		{
			KDataMuon muon;
			for (KDataMuons::const_iterator it = product.m_listValidMuons.begin();
				 it != product.m_listValidMuons.end(); it ++)
			{
				if ((it == product.m_listValidMuons.begin()) || (it->p4.Pt() > muon.p4.Pt()))
					muon = *it;
			}
			return muon.p4.Eta();
		}
		else if (e == E::mu2pt)
		{
			KDataMuon muon;
			for (KDataMuons::const_iterator it = product.m_listValidMuons.begin();
				 it != product.m_listValidMuons.end(); it ++)
			{
				if ((it == product.m_listValidMuons.begin()) || (it->p4.Pt() < muon.p4.Pt()))
					muon = *it;
			}
			return muon.p4.Pt();
		}
		else if (e == E::mu2phi)
		{
			KDataMuon muon;
			for (KDataMuons::const_iterator it = product.m_listValidMuons.begin();
				 it != product.m_listValidMuons.end(); it ++)
			{
				if ((it == product.m_listValidMuons.begin()) || (it->p4.Pt() < muon.p4.Pt()))
					muon = *it;
			}
			return muon.p4.Phi();
		}
		else if (e == E::mu2eta)
		{
			KDataMuon muon;
			for (KDataMuons::const_iterator it = product.m_listValidMuons.begin();
				 it != product.m_listValidMuons.end(); it ++)
			{
				if ((it == product.m_listValidMuons.begin()) || (it->p4.Pt() < muon.p4.Pt()))
					muon = *it;
			}
			return muon.p4.Eta();
		}
		else if (e == E::genmupluspt)
		{
			for (const auto & it : product.m_genMuons)
				if (it.charge() > 0) return it.p4.Pt();
			return -999;
		}
		else if (e == E::genmupluseta)
		{
			for (const auto & it : product.m_genMuons)
				if (it.charge() > 0) return it.p4.Eta();
			return -999;
		}
		else if (e == E::genmuplusphi)
		{
			for (const auto & it : product.m_genMuons)
				if (it.charge() > 0) return it.p4.Phi();
			return -999;
		}
		else if (e == E::genmuminuspt)
		{
			for (const auto & it : product.m_genMuons)
				if (it.charge() < 0) return it.p4.Pt();
			return -999;
		}
		else if (e == E::genmuminuseta)
		{
			for (const auto & it : product.m_genMuons)
				if (it.charge() < 0) return it.p4.Eta();
			return -999;
		}
		else if (e == E::genmuminusphi)
		{
			for (const auto & it : product.m_genMuons)
				if (it.charge() < 0) return it.p4.Phi();
			return -999;
		}
		else if (e == E::nmuons)
			return product.m_listValidMuons.size();
		else if (e == E::ngenmuons)
			return product.m_genMuons.size();

		//gen electron
		else if (e == E::genepluspt)
		{
			for (const auto & it : product.m_genInternalElectrons)
				if (it.charge() > 0) return it.p4.Pt();
			return -999;
		}
		else if (e == E::genepluseta)
		{
			for (const auto & it : product.m_genInternalElectrons)
				if (it.charge() > 0) return it.p4.Eta();
			return -999;
		}
		else if (e == E::geneplusphi)
		{
			for (const auto & it : product.m_genInternalElectrons)
				if (it.charge() > 0) return it.p4.Phi();
			return -999;
		}
		else if (e == E::geneminuspt)
		{
			for (const auto & it : product.m_genInternalElectrons)
				if (it.charge() < 0) return it.p4.Pt();
			return -999;
		}
		else if (e == E::geneminuseta)
		{
			for (const auto & it : product.m_genInternalElectrons)
				if (it.charge() < 0) return it.p4.Eta();
			return -999;
		}
		else if (e == E::geneminusphi)
		{
			for (const auto & it : product.m_genInternalElectrons)
				if (it.charge() < 0) return it.p4.Phi();
			return -999;
		}
		else if (e == E::ngenelectrons)
			return product.m_genElectrons.size();
		else if (e == E::ngeninternalelectrons)
			return product.m_genInternalElectrons.size();
		else if (e == E::ngenintermediateelectrons)
			return product.m_genIntermediateElectrons.size();

		// jets
		else if (e == E::njets)
			return product.GetValidJetCount(s, event);
		else if (e == E::njets30) // needed for Zee studies
		{
			float count = 0.;
			for (unsigned int i = 0; i < product.GetValidJetCount(s, event); i++)
			{
				if (product.GetValidJet(s, event, i)->p4.Pt() > 30)
					count += 1.;
			}
			return count;
		}
		else if (e == E::njets30barrel) // needed for Zee studies
		{
			float count = 0.;
			for (unsigned int i = 0; i < product.GetValidJetCount(s, event); i++)
			{
				if (product.GetValidJet(s, event, i)->p4.Pt() > 30 && std::abs(product.GetValidJet(s, event, i)->p4.Eta()) < 2.4)
					count += 1.;
			}
			return count;
		}
		else if (e == E::njetsinv)
			return product.m_listInvalidJets["AK5PFJetsCHS"].size();
		else if (e == E::ngenphotons)
			return product.m_genPhotons.size();
		else if (e == E::nzs)
			return product.m_genZs.size();
		else if (e == E::ninternalmuons)
			return product.m_genInternalMuons.size();
		else if (e == E::nintermediatemuons)
			return product.m_genIntermediateMuons.size();
		else if (e == E::closestphotondr)
		{
			double dR = 999.;
			double d = dR;
			for (const auto & mu : product.m_genMuons)
				for (const auto & ph : product.m_genPhotons)
				{
					d = ROOT::Math::VectorUtil::DeltaR(mu.p4, ph.p4);
					if (d < dR && ph.p4.Pt() > 1)
						dR = d;
				}
			return dR;
		}
		else if (e == E::ngenphotonsclose)
		{
			const double dR = 0.3;
			int n = 0;
			for (const auto & mu : product.m_genMuons)
				for (const auto & ph : product.m_genPhotons)
					if (ROOT::Math::VectorUtil::DeltaR(mu.p4, ph.p4) < dR)
						n++;
			return n;
		}
		else if (e == E::ptgenphotonsclose)
		{
			const double dR = 0.3;
			double pt = 0;
			for (const auto & mu : product.m_genMuons)
				for (const auto & ph : product.m_genPhotons)
					if (ROOT::Math::VectorUtil::DeltaR(mu.p4, ph.p4) < dR)
						pt += ph.p4.Pt();
			return pt;
		}
		else if (e == E::ptgenphotonsfar)
		{
			const double dR = 0.3;
			double pt = 0;
			for (const auto & mu : product.m_genMuons)
			{
				// look at a cone perpendicular to the muon
				KGenParticle nomu = mu;
				nomu.p4.SetPhi(nomu.p4.Phi() + ROOT::Math::Pi() / 2);
				for (const auto & ph : product.m_genPhotons)
					if (ROOT::Math::VectorUtil::DeltaR(nomu.p4, ph.p4) < dR)
						pt += ph.p4.Pt();
			}
			return pt;
		}
		else if (e == E::ptdiff13)
		{
			if (product.m_genMuons.size() < 2 || product.m_genInternalMuons.size() < 2)
				return -999;
			double diff = 0;
			diff = product.m_genMuons[0].p4.Pt()
				   + product.m_genMuons[1].p4.Pt()
				   - product.m_genInternalMuons[0].p4.Pt()
				   - product.m_genInternalMuons[1].p4.Pt();
			return diff;
		}
		else if (e == E::ptdiff12)
		{
			if (product.m_genMuons.size() < 2 || product.m_genIntermediateMuons.size() < 2)
				return -999;
			double diff = 0;
			diff = product.m_genMuons[0].p4.Pt()
				   + product.m_genMuons[1].p4.Pt()
				   - product.m_genIntermediateMuons[0].p4.Pt()
				   - product.m_genIntermediateMuons[1].p4.Pt();
			return diff;
		}
		else if (e == E::ptdiff23)
		{
			if (product.m_genIntermediateMuons.size() < 2 || product.m_genInternalMuons.size() < 2)
				return -999;
			double diff = 0;
			diff = product.m_genMuons[0].p4.Pt()
				   + product.m_genMuons[1].p4.Pt()
				   - product.m_genIntermediateMuons[0].p4.Pt()
				   - product.m_genIntermediateMuons[1].p4.Pt();
			return diff;
		}
		else if (e == E::genzpt)
		{
			if (product.m_genZs.size() < 1)
				return -1;
			return product.m_genZs[0].p4.Pt();
		}
		else if (e == E::genzy)
		{
			if (product.m_genZs.size() < 1)
				return -1;
			return product.m_genZs[0].p4.Rapidity();
		}
		else if (e == E::genzmass)
		{
			if (product.m_genZs.size() < 1)
				return -1;
			return product.m_genZs[0].p4.mass();
		}
		else if (e == E::deltaRzgenz)
		{
			if (product.m_genZs.size() < 1)
				return -1;
			else
				return ROOT::Math::VectorUtil::DeltaR(product.m_genZs[0].p4,
													  product.GetRefZ().p4);
		}
		else if (e == E::deltaReplusgeneplus)
		{

			for (const auto & it : product.m_genInternalElectrons)
			{
				if (it.charge() > 0)
					return ROOT::Math::VectorUtil::DeltaR(product.leadingeplus.p4, it.p4);
			}
			return 999;
		}
		else if (e == E::deltaReminusgeneminus)
		{

			for (const auto & it : product.m_genInternalElectrons)
			{
				if (it.charge() < 0)
					return ROOT::Math::VectorUtil::DeltaR(product.leadingeminus.p4, it.p4);
			}
			return 999;
		}

		//LHE information
		else if (e == E::lhezpt)
			return product.GetLHEZ().p4.Pt();
		else if (e == E::lhezeta)
			return product.GetLHEZ().p4.Eta();
		else if (e == E::lhezy)
			return product.GetLHEZ().p4.Rapidity();
		else if (e == E::lhezphi)
			return product.GetLHEZ().p4.Phi();
		else if (e == E::lhezmass)
			return product.GetLHEZ().p4.mass();
		else if (e == E::nlhemuons)
			return product.m_nLHEMuons;
		else if (e == E::nlheelectrons)
			return product.m_nLHEElectrons;
		else if (e == E::nlhetaus)
			return product.m_nLHETaus;


		// electrons
		else if (e == E::nelectrons)
			return product.GetValidElectrons().size();

		else if (e == E::emass)
			return product.leadinge.p4.mass();
		else if (e == E::ept)
			return product.leadinge.p4.Pt();
		else if (e == E::eeta)
			return product.leadinge.p4.Eta();
		else if (e == E::eminusmass)
			return product.leadingeminus.p4.mass();
		else if (e == E::eminuspt)
			return product.leadingeminus.p4.Pt();
		else if (e == E::eminuseta)
			return product.leadingeminus.p4.Eta();
		else if (e == E::eminusphi)
			return product.leadingeminus.p4.Phi();
		else if (e == E::eplusmass)
			return product.leadingeplus.p4.mass();
		else if (e == E::epluspt)
			return product.leadingeplus.p4.Pt();
		else if (e == E::epluseta)
			return product.leadingeplus.p4.Eta();
		else if (e == E::eplusphi)
			return product.leadingeplus.p4.Phi();



		else if (e == E::eminusiso)
			return product.leadingeminus.trackIso03;
		else if (e == E::eplusiso)
			return product.leadingeplus.trackIso03;

		else if (e == E::eminusecaliso03)
			return product.leadingeminus.ecalIso03;
		else if (e == E::eplusecaliso03)
			return product.leadingeplus.ecalIso03;

		else if (e == E::eminusecaliso04)
			return product.leadingeminus.ecalIso04;
		else if (e == E::eplusecaliso04)
			return product.leadingeplus.ecalIso04;

		else if (e == E::eminusid)
			return product.leadingeminus.idMvaNonTrigV0;
		else if (e == E::eplusid)
			return product.leadingeplus.idMvaNonTrigV0;

		else if (e == E::eminustrigid)
			return product.leadingeminus.idMvaTrigV0;
		else if (e == E::eplustrigid)
			return product.leadingeplus.idMvaTrigV0;


		// leading jet
		else if (e == E::jet1pt)
			return product.GetValidPrimaryJet(s, event)->p4.Pt();
		else if (e == E::jet1eta)
			return product.GetValidPrimaryJet(s, event)->p4.Eta();
		else if (e == E::jet1y)
			return product.GetValidPrimaryJet(s, event)->p4.Rapidity();
		else if (e == E::jet1phi)
			return product.GetValidPrimaryJet(s, event)->p4.Phi();

		// leading jet composition
		else if (e == E::jet1photonfraction)
			return static_cast<KDataPFJet*>(product.GetValidPrimaryJet(s, event))->photonFraction;
		else if (e == E::jet1chargedemfraction)
			return static_cast<KDataPFJet*>(product.GetValidPrimaryJet(s, event))->chargedEMFraction;
		else if (e == E::jet1chargedhadfraction)
			return static_cast<KDataPFJet*>(product.GetValidPrimaryJet(s, event))->chargedHadFraction;
		else if (e == E::jet1neutralhadfraction)
			return static_cast<KDataPFJet*>(product.GetValidPrimaryJet(s, event))->neutralHadFraction;
		else if (e == E::jet1muonfraction)
			return static_cast<KDataPFJet*>(product.GetValidPrimaryJet(s, event))->muonFraction;
		else if (e == E::jet1HFhadfraction)
			return static_cast<KDataPFJet*>(product.GetValidPrimaryJet(s, event))->HFHadFraction;
		else if (e == E::jet1HFemfraction)
			return static_cast<KDataPFJet*>(product.GetValidPrimaryJet(s, event))->HFEMFraction;

		// second jet
		else if (e == E::jet2pt)
		{
			if (product.GetValidJetCount(s, event) > 1)
				return product.GetValidJet(s, event, 1)->p4.Pt();
			else
				return 0;
		}
		else if (e == E::jet2phi)
		{
			if (product.GetValidJetCount(s, event) > 1)
				return product.GetValidJet(s, event, 1)->p4.Phi();
			else
				return 0;
		}
		else if (e == E::jet2eta)
		{
			if (product.GetValidJetCount(s, event) > 1)
				return product.GetValidJet(s, event, 1)->p4.Eta();
			else
				return 0;
		}

		// MET & sumEt
		else if (e == E::METpt)
			return product.GetMet(event, s)->p4.Pt();
		else if (e == E::METphi)
			return product.GetMet(event, s)->p4.Phi();
		else if (e == E::sumEt)
			return product.GetMet(event, s)->sumEt;
		else if (e == E::rawMETpt)
			return event.GetMet(s)->p4.Pt();
		else if (e == E::rawMETphi)
			return event.GetMet(s)->p4.Phi();

		else if (e == E::uept)
			return product.GetUE(event, s)->p4.Pt();
		else if (e == E::uephi)
			return product.GetUE(event, s)->p4.Phi();
		else if (e == E::ueeta)
			return product.GetUE(event, s)->p4.Eta();
		else if (e == E::mpf)
			return product.GetMPF(product.GetMet(event, s));
		else if (e == E::rawmpf)
			return product.GetMPF(event.GetMet(s));
		else if (e == E::otherjetspt)
		{
			if (product.GetValidJetCount(s, event) < 2)
				return 0;
			else
				return (-(product.GetRefZ().p4
						  + product.GetValidPrimaryJet(s, event)->p4
						  + product.GetMet(event, s)->p4
						  + product.GetValidJet(s, event, 1)->p4
						  + product.GetUE(event, s)->p4
						 )).Pt();
		}
		else if (e == E::otherjetsphi)
		{
			if (product.GetValidJetCount(s, event) < 2)
				return 0;
			else
				return (-(product.GetRefZ().p4
						  + product.GetValidPrimaryJet(s, event)->p4
						  + product.GetMet(event, s)->p4
						  + product.GetValidJet(s, event, 1)->p4
						  + product.GetUE(event, s)->p4
						 )).Phi();
		}
		else if (e == E::otherjetseta)
		{
			if (product.GetValidJetCount(s, event) < 2)
				return 0;
			else
				return (-(product.GetRefZ().p4
						  + product.GetValidPrimaryJet(s, event)->p4
						  + product.GetMet(event, s)->p4
						  + product.GetValidJet(s, event, 1)->p4
						  + product.GetUE(event, s)->p4
						 )).Eta();
		}
		//gen jets
		else if (e == E::genjet1pt)
		{
			std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));

			if (product.GetValidJetCount(s, event, genName) == 0)
				return false;

			return product.GetValidJet(s, event, 0, genName)->p4.Pt();
		}
		else if (e == E::genjet1ptneutrinos)
		{
			std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));

			if (product.GetValidJetCount(s, event, genName) == 0)
				return false;

			KDataLV v = * product.GetValidJet(s, event, 0, genName);
			if (product.m_neutrinos[genName].size() > 0)
				for (const auto & it : product.m_neutrinos[genName])
					v.p4 += it.p4;
			return v.p4.Pt();
		}
		else if (e == E::genjet1eta)
		{
			std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));

			if (product.GetValidJetCount(s, event, genName) == 0)
				return false;

			return product.GetValidJet(s, event, 0, genName)->p4.Eta();
		}
		else if (e == E::genjet1phi)
		{
			std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));

			if (product.GetValidJetCount(s, event, genName) == 0)
				return false;

			return product.GetValidJet(s, event, 0, genName)->p4.Phi();
		}
		else if (e == E::matchedgenjet1pt)
		{
			return product.GetMatchedGenJet(event, s, 0)->p4.Pt();
		}
		else if (e == E::matchedgenjet2pt)
		{
			return product.GetMatchedGenJet(event, s, 1)->p4.Pt();
		}
		else if (e == E::genjet2pt)
		{
			std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));

			if (product.GetValidJetCount(s, event, genName) < 2)
				return false;

			return product.GetValidJet(s, event, 1, genName)->p4.Pt();
		}
		else if (e == E::genmpf)
			return product.GetGenMPF(product.GetPtGenMet());
		else if (e == E::algoflavour)
			return product.GetAlgoFlavour(s);
		else if (e == E::physflavour)
			return product.GetPhysFlavour(s);
		else if (e == E::algopt)
			return product.GetAlgoPt(s);
		else if (e == E::physpt)
			return product.GetPhysPt(s);
		else if (e == E::eventnr)
			return event.m_eventproduct->nEvent;
		else if (e == E::lumisec)
			return event.m_eventproduct->nLumi;
		else if (e == E::jet1ptneutrinos)
		{
			KDataLV v = * product.GetValidPrimaryJet(s, event);
			std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));
			if (product.m_neutrinos[genName].size() > 0)
				for (const auto & it : product.m_neutrinos[genName])
					v.p4 += it.p4;
			return v.p4.Pt();
		}
		else if (e == E::mpfneutrinos)
		{
			KDataPFMET met = * product.GetMet(event, s);
			std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));
			if (product.m_neutrinos[genName].size() > 0)
				for (const auto & it : product.m_neutrinos[genName])
					met.p4 -= it.p4;
			met.p4.SetEta(0);
			return product.GetMPF(&met);
		}
		else if (e == E::neutralpt3)
		{
			KDataLV v;
			std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));
			if (product.m_neutrals3[genName].size() > 0)
				for (const auto & it : product.m_neutrals3[genName])
					v.p4 += it.p4;
			return v.p4.Pt();
		}
		else if (e == E::neutralpt5)
		{
			KDataLV v;
			std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));
			if (product.m_neutrals5[genName].size() > 0)
				for (const auto & it : product.m_neutrals5[genName])
					v.p4 += it.p4;
			return v.p4.Pt();
		}
		else if (e == E::unc)
			return product.leadingjetuncertainty[s.GetJetAlgorithm()];
		else if (e == E::hlt)
		{
			if (product.GetSelectedHlt().empty()) // no HLT found
				return 0;
			else
				return event.m_eventproduct->hltFired(product.GetSelectedHlt(), event.m_lumiproduct);
		}

		//some electron stuff
		else if (e == E::sf)
			return product.scalefactor;
		else if (e == E::sfplus)
			return product.sfplus;
		else if (e == E::sfminus)
			return product.sfminus;
		else if (e == E::eidveto)
			return product.electronidveto;
		else if (e == E::eplusidloose)
			return product.leadingeplus.cutbasedIDloose;
		else if (e == E::eplusidmedium)
			return product.leadingeplus.cutbasedIDmedium;
		else if (e == E::eplusidtight)
			return product.leadingeplus.cutbasedIDtight;
		else if (e == E::eplusidveto)
			return product.leadingeplus.cutbasedIDveto;
		else if (e == E::eminusidloose)
			return product.leadingeminus.cutbasedIDloose;
		else if (e == E::eminusidmedium)
			return product.leadingeminus.cutbasedIDmedium;
		else if (e == E::eminusidtight)
			return product.leadingeminus.cutbasedIDtight;
		else if (e == E::eminusidveto)
			return product.leadingeminus.cutbasedIDveto;

		// some stuff for flavour corrections
		else if (e == E::algol5pt || e == E::physl5pt)
		{
			int flavour;
			if (e == E::algol5pt)
				flavour = std::abs(product.GetAlgoFlavour(s));
			else if (e == E::physl5pt)
				flavour = std::abs(product.GetPhysFlavour(s));

			if (flavour == 21)
				return product.m_validPFJets.at(s.GetJetAlgorithm() + "L5g").at(0).p4.Pt();
			else if (flavour == 4)
				return product.m_validPFJets.at(s.GetJetAlgorithm() + "L5c").at(0).p4.Pt();
			else if (flavour == 5)
				return product.m_validPFJets.at(s.GetJetAlgorithm() + "L5b").at(0).p4.Pt();
			else if (flavour == 5)
				return product.m_validPFJets.at(s.GetJetAlgorithm() + "L5b").at(0).p4.Pt();
			else if (flavour > 0 && flavour < 4)
				return product.m_validPFJets.at(s.GetJetAlgorithm() + "L5q").at(0).p4.Pt();
			else
				return product.GetValidPrimaryJet(s, event)->p4.Pt();
		}
		else if (e == E::mpfalgo)
			return product.GetMPF(&product.m_MET[s.GetJetAlgorithm() + "L5Algo"]);
		else if (e == E::mpfphys)
			return product.GetMPF(&product.m_MET[s.GetJetAlgorithm() + "L5Phys"]);
		else if (e == E::mpfneutrinosalgo)
		{
			KDataPFMET met = product.m_MET[s.GetJetAlgorithm() + "L5Algo"];
			std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));
			if (product.m_neutrinos[genName].size() > 0)
				for (const auto & it : product.m_neutrinos[genName])
					met.p4 -= it.p4;
			met.p4.SetEta(0);
			return product.GetMPF(&met);
		}
		else if (e == E::mpfneutrinosphys)
		{
			KDataPFMET met = product.m_MET[s.GetJetAlgorithm() + "L5Phys"];
			std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));
			if (product.m_neutrinos[genName].size() > 0)
				for (const auto & it : product.m_neutrinos[genName])
					met.p4 -= it.p4;
			met.p4.SetEta(0);
			return product.GetMPF(&met);
		}
		else if (e == E::jet1ptneutrinosalgo)
		{

			KDataLV v = * product.GetValidJet(s, event, 0, s.GetJetAlgorithm() + "L5Algo");
			std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));
			if (product.m_neutrinos[genName].size() > 0)
				for (const auto & it : product.m_neutrinos[genName])
					v.p4 += it.p4;
			return v.p4.Pt();
		}
		else if (e == E::jet1ptneutrinosphys)
		{

			KDataLV v = * product.GetValidJet(s, event, 0, s.GetJetAlgorithm() + "L5Phys");
			std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));
			if (product.m_neutrinos[genName].size() > 0)
				for (const auto & it : product.m_neutrinos[genName])
					v.p4 += it.p4;
			return v.p4.Pt();
		}

		LOG_FATAL("None found");
		assert(false);
		return -999;
	};

	void fillEnumVector(std::vector<std::string> const& quantities)
	{
		for (auto string : quantities)
		{
			if (string == "npv")
				m_enumvector.push_back(E::npv);
			else if (string == "npu")
				m_enumvector.push_back(E::npu);
			else if (string == "nputruth")
				m_enumvector.push_back(E::nputruth);
			else if (string == "qglikelihood")
				m_enumvector.push_back(E::qglikelihood);
			else if (string == "qgmlp")
				m_enumvector.push_back(E::qgmlp);
			else if (string == "trackcountinghigheffbjettag")
				m_enumvector.push_back(E::trackcountinghigheffbjettag);
			else if (string == "trackcountinghighpurbjettag")
				m_enumvector.push_back(E::trackcountinghighpurbjettag);
			else if (string == "jetprobabilitybjettag")
				m_enumvector.push_back(E::jetprobabilitybjettag);
			else if (string == "jetbprobabilitybjettag")
				m_enumvector.push_back(E::jetbprobabilitybjettag);
			else if (string == "softelectronbjettag")
				m_enumvector.push_back(E::softelectronbjettag);
			else if (string == "softmuonbjettag")
				m_enumvector.push_back(E::softmuonbjettag);
			else if (string == "softmuonbyip3dbjettag")
				m_enumvector.push_back(E::softmuonbyip3dbjettag);
			else if (string == "softmuonbyptbjettag")
				m_enumvector.push_back(E::softmuonbyptbjettag);
			else if (string == "simplesecondaryvertexbjettag")
				m_enumvector.push_back(E::simplesecondaryvertexbjettag);
			else if (string == "combinedsecondaryvertexbjettag")
				m_enumvector.push_back(E::combinedsecondaryvertexbjettag);
			else if (string == "combinedsecondaryvertexmvabjettag")
				m_enumvector.push_back(E::combinedsecondaryvertexmvabjettag);
			else if (string == "jet1puJetIDFullTight")
				m_enumvector.push_back(E::jet1puJetIDFullTight);
			else if (string == "jet1puJetIDFullMedium")
				m_enumvector.push_back(E::jet1puJetIDFullMedium);
			else if (string == "jet1puJetIDFullLoose")
				m_enumvector.push_back(E::jet1puJetIDFullLoose);
			else if (string == "jet1puJetIDCutbasedTight")
				m_enumvector.push_back(E::jet1puJetIDCutbasedTight);
			else if (string == "jet1puJetIDCutbasedMedium")
				m_enumvector.push_back(E::jet1puJetIDCutbasedMedium);
			else if (string == "jet1puJetIDCutbasedLoose")
				m_enumvector.push_back(E::jet1puJetIDCutbasedLoose);
			else if (string == "jet2puJetIDFullTight")
				m_enumvector.push_back(E::jet2puJetIDFullTight);
			else if (string == "jet2puJetIDFullMedium")
				m_enumvector.push_back(E::jet2puJetIDFullMedium);
			else if (string == "jet2puJetIDFullLoose")
				m_enumvector.push_back(E::jet2puJetIDFullLoose);
			else if (string == "jet2puJetIDCutbasedTight")
				m_enumvector.push_back(E::jet2puJetIDCutbasedTight);
			else if (string == "jet2puJetIDCutbasedMedium")
				m_enumvector.push_back(E::jet2puJetIDCutbasedMedium);
			else if (string == "jet2puJetIDCutbasedLoose")
				m_enumvector.push_back(E::jet2puJetIDCutbasedLoose);
			else if (string == "rho")
				m_enumvector.push_back(E::rho);
			else if (string == "run")
				m_enumvector.push_back(E::run);
			else if (string == "weight")
				m_enumvector.push_back(E::weight);
			else if (string == "eff")
				m_enumvector.push_back(E::eff);
			else if (string == "zpt")
				m_enumvector.push_back(E::zpt);
			else if (string == "zeta")
				m_enumvector.push_back(E::zeta);
			else if (string == "zphi")
				m_enumvector.push_back(E::zphi);
			else if (string == "zy")
				m_enumvector.push_back(E::zy);
			else if (string == "zmass")
				m_enumvector.push_back(E::zmass);
			else if (string == "mupluspt")
				m_enumvector.push_back(E::mupluspt);
			else if (string == "mupluseta")
				m_enumvector.push_back(E::mupluseta);
			else if (string == "muplusphi")
				m_enumvector.push_back(E::muplusphi);
			else if (string == "muminuspt")
				m_enumvector.push_back(E::muminuspt);
			else if (string == "muminuseta")
				m_enumvector.push_back(E::muminuseta);
			else if (string == "muminusphi")
				m_enumvector.push_back(E::muminusphi);
			else if (string == "muplusiso")
				m_enumvector.push_back(E::muplusiso);
			else if (string == "muminusiso")
				m_enumvector.push_back(E::muminusiso);
			else if (string == "mu1pt")
				m_enumvector.push_back(E::mu1pt);
			else if (string == "mu1phi")
				m_enumvector.push_back(E::mu1phi);
			else if (string == "mu1eta")
				m_enumvector.push_back(E::mu1eta);
			else if (string == "mu2pt")
				m_enumvector.push_back(E::mu2pt);
			else if (string == "mu2phi")
				m_enumvector.push_back(E::mu2phi);
			else if (string == "mu2eta")
				m_enumvector.push_back(E::mu2eta);
			else if (string == "genmupluspt")
				m_enumvector.push_back(E::genmupluspt);
			else if (string == "genmupluseta")
				m_enumvector.push_back(E::genmupluseta);
			else if (string == "genmuplusphi")
				m_enumvector.push_back(E::genmuplusphi);
			else if (string == "genmuminuspt")
				m_enumvector.push_back(E::genmuminuspt);
			else if (string == "genmuminuseta")
				m_enumvector.push_back(E::genmuminuseta);
			else if (string == "genmuminusphi")
				m_enumvector.push_back(E::genmuminusphi);
			else if (string == "nmuons")
				m_enumvector.push_back(E::nmuons);
			else if (string == "ngenmuons")
				m_enumvector.push_back(E::ngenmuons);
			else if (string == "genepluspt")
				m_enumvector.push_back(E::genepluspt);
			else if (string == "genepluseta")
				m_enumvector.push_back(E::genepluseta);
			else if (string == "geneplusphi")
				m_enumvector.push_back(E::geneplusphi);
			else if (string == "geneminuspt")
				m_enumvector.push_back(E::geneminuspt);
			else if (string == "geneminuseta")
				m_enumvector.push_back(E::geneminuseta);
			else if (string == "geneminusphi")
				m_enumvector.push_back(E::geneminusphi);
			else if (string == "ngenelectrons")
				m_enumvector.push_back(E::ngenelectrons);
			else if (string == "ngeninternalelectrons")
				m_enumvector.push_back(E::ngeninternalelectrons);
			else if (string == "ngenintermediateelectrons")
				m_enumvector.push_back(E::ngenintermediateelectrons);
			else if (string == "njets")
				m_enumvector.push_back(E::njets);
			else if (string == "njets30")
				m_enumvector.push_back(E::njets30);
			else if (string == "njets30barrel")
				m_enumvector.push_back(E::njets30barrel);
			else if (string == "njetsinv")
				m_enumvector.push_back(E::njetsinv);
			else if (string == "ngenphotons")
				m_enumvector.push_back(E::ngenphotons);
			else if (string == "nzs")
				m_enumvector.push_back(E::nzs);
			else if (string == "ninternalmuons")
				m_enumvector.push_back(E::ninternalmuons);
			else if (string == "nintermediatemuons")
				m_enumvector.push_back(E::nintermediatemuons);
			else if (string == "closestphotondr")
				m_enumvector.push_back(E::closestphotondr);
			else if (string == "ngenphotonsclose")
				m_enumvector.push_back(E::ngenphotonsclose);
			else if (string == "ptgenphotonsclose")
				m_enumvector.push_back(E::ptgenphotonsclose);
			else if (string == "ptgenphotonsfar")
				m_enumvector.push_back(E::ptgenphotonsfar);
			else if (string == "ptdiff13")
				m_enumvector.push_back(E::ptdiff13);
			else if (string == "ptdiff12")
				m_enumvector.push_back(E::ptdiff12);
			else if (string == "ptdiff23")
				m_enumvector.push_back(E::ptdiff23);
			else if (string == "genzpt")
				m_enumvector.push_back(E::genzpt);
			else if (string == "genzy")
				m_enumvector.push_back(E::genzy);
			else if (string == "genzmass")
				m_enumvector.push_back(E::genzmass);
			else if (string == "deltaRzgenz")
				m_enumvector.push_back(E::deltaRzgenz);
			else if (string == "deltaReplusgeneplus")
				m_enumvector.push_back(E::deltaReplusgeneplus);
			else if (string == "deltaReminusgeneminus")
				m_enumvector.push_back(E::deltaReminusgeneminus);
			else if (string == "lhezpt")
				m_enumvector.push_back(E::lhezpt);
			else if (string == "lhezeta")
				m_enumvector.push_back(E::lhezeta);
			else if (string == "lhezy")
				m_enumvector.push_back(E::lhezy);
			else if (string == "lhezphi")
				m_enumvector.push_back(E::lhezphi);
			else if (string == "lhezmass")
				m_enumvector.push_back(E::lhezmass);
			else if (string == "nlhemuons")
				m_enumvector.push_back(E::nlhemuons);
			else if (string == "nlheelectrons")
				m_enumvector.push_back(E::nlheelectrons);
			else if (string == "nlhetaus")
				m_enumvector.push_back(E::nlhetaus);
			else if (string == "nelectrons")
				m_enumvector.push_back(E::nelectrons);
			else if (string == "emass")
				m_enumvector.push_back(E::emass);
			else if (string == "ept")
				m_enumvector.push_back(E::ept);
			else if (string == "eeta")
				m_enumvector.push_back(E::eeta);
			else if (string == "eminusmass")
				m_enumvector.push_back(E::eminusmass);
			else if (string == "eminuspt")
				m_enumvector.push_back(E::eminuspt);
			else if (string == "eminuseta")
				m_enumvector.push_back(E::eminuseta);
			else if (string == "eminusphi")
				m_enumvector.push_back(E::eminusphi);
			else if (string == "eplusmass")
				m_enumvector.push_back(E::eplusmass);
			else if (string == "epluspt")
				m_enumvector.push_back(E::epluspt);
			else if (string == "epluseta")
				m_enumvector.push_back(E::epluseta);
			else if (string == "eplusphi")
				m_enumvector.push_back(E::eplusphi);
			else if (string == "eminusiso")
				m_enumvector.push_back(E::eminusiso);
			else if (string == "eplusiso")
				m_enumvector.push_back(E::eplusiso);
			else if (string == "eminusecaliso03")
				m_enumvector.push_back(E::eminusecaliso03);
			else if (string == "eplusecaliso03")
				m_enumvector.push_back(E::eplusecaliso03);
			else if (string == "eminusecaliso04")
				m_enumvector.push_back(E::eminusecaliso04);
			else if (string == "eplusecaliso04")
				m_enumvector.push_back(E::eplusecaliso04);
			else if (string == "eminusid")
				m_enumvector.push_back(E::eminusid);
			else if (string == "eplusid")
				m_enumvector.push_back(E::eplusid);
			else if (string == "eminustrigid")
				m_enumvector.push_back(E::eminustrigid);
			else if (string == "eplustrigid")
				m_enumvector.push_back(E::eplustrigid);
			else if (string == "jet1pt")
				m_enumvector.push_back(E::jet1pt);
			else if (string == "jet1eta")
				m_enumvector.push_back(E::jet1eta);
			else if (string == "jet1y")
				m_enumvector.push_back(E::jet1y);
			else if (string == "jet1phi")
				m_enumvector.push_back(E::jet1phi);
			else if (string == "jet1photonfraction")
				m_enumvector.push_back(E::jet1photonfraction);
			else if (string == "jet1chargedemfraction")
				m_enumvector.push_back(E::jet1chargedemfraction);
			else if (string == "jet1chargedhadfraction")
				m_enumvector.push_back(E::jet1chargedhadfraction);
			else if (string == "jet1neutralhadfraction")
				m_enumvector.push_back(E::jet1neutralhadfraction);
			else if (string == "jet1muonfraction")
				m_enumvector.push_back(E::jet1muonfraction);
			else if (string == "jet1HFhadfraction")
				m_enumvector.push_back(E::jet1HFhadfraction);
			else if (string == "jet1HFemfraction")
				m_enumvector.push_back(E::jet1HFemfraction);
			else if (string == "jet2pt")
				m_enumvector.push_back(E::jet2pt);
			else if (string == "jet2phi")
				m_enumvector.push_back(E::jet2phi);
			else if (string == "jet2eta")
				m_enumvector.push_back(E::jet2eta);
			else if (string == "METpt")
				m_enumvector.push_back(E::METpt);
			else if (string == "METphi")
				m_enumvector.push_back(E::METphi);
			else if (string == "sumEt")
				m_enumvector.push_back(E::sumEt);
			else if (string == "rawMETpt")
				m_enumvector.push_back(E::rawMETpt);
			else if (string == "rawMETphi")
				m_enumvector.push_back(E::rawMETphi);
			else if (string == "uept")
				m_enumvector.push_back(E::uept);
			else if (string == "uephi")
				m_enumvector.push_back(E::uephi);
			else if (string == "ueeta")
				m_enumvector.push_back(E::ueeta);
			else if (string == "mpf")
				m_enumvector.push_back(E::mpf);
			else if (string == "rawmpf")
				m_enumvector.push_back(E::rawmpf);
			else if (string == "otherjetspt")
				m_enumvector.push_back(E::otherjetspt);
			else if (string == "otherjetsphi")
				m_enumvector.push_back(E::otherjetsphi);
			else if (string == "otherjetseta")
				m_enumvector.push_back(E::otherjetseta);
			else if (string == "genjet1pt")
				m_enumvector.push_back(E::genjet1pt);
			else if (string == "genjet1ptneutrinos")
				m_enumvector.push_back(E::genjet1ptneutrinos);
			else if (string == "genjet1eta")
				m_enumvector.push_back(E::genjet1eta);
			else if (string == "genjet1phi")
				m_enumvector.push_back(E::genjet1phi);
			else if (string == "matchedgenjet1pt")
				m_enumvector.push_back(E::matchedgenjet1pt);
			else if (string == "matchedgenjet2pt")
				m_enumvector.push_back(E::matchedgenjet2pt);
			else if (string == "genjet2pt")
				m_enumvector.push_back(E::genjet2pt);
			else if (string == "genmpf")
				m_enumvector.push_back(E::genmpf);
			else if (string == "algoflavour")
				m_enumvector.push_back(E::algoflavour);
			else if (string == "physflavour")
				m_enumvector.push_back(E::physflavour);
			else if (string == "algopt")
				m_enumvector.push_back(E::algopt);
			else if (string == "physpt")
				m_enumvector.push_back(E::physpt);
			else if (string == "eventnr")
				m_enumvector.push_back(E::eventnr);
			else if (string == "lumisec")
				m_enumvector.push_back(E::lumisec);
			else if (string == "jet1ptneutrinos")
				m_enumvector.push_back(E::jet1ptneutrinos);
			else if (string == "mpfneutrinos")
				m_enumvector.push_back(E::mpfneutrinos);
			else if (string == "neutralpt3")
				m_enumvector.push_back(E::neutralpt3);
			else if (string == "neutralpt5")
				m_enumvector.push_back(E::neutralpt5);
			else if (string == "unc")
				m_enumvector.push_back(E::unc);
			else if (string == "hlt")
				m_enumvector.push_back(E::hlt);
			else if (string == "sf")
				m_enumvector.push_back(E::sf);
			else if (string == "sfplus")
				m_enumvector.push_back(E::sfplus);
			else if (string == "sfminus")
				m_enumvector.push_back(E::sfminus);
			else if (string == "eidveto")
				m_enumvector.push_back(E::eidveto);
			else if (string == "eplusidloose")
				m_enumvector.push_back(E::eplusidloose);
			else if (string == "eplusidmedium")
				m_enumvector.push_back(E::eplusidmedium);
			else if (string == "eplusidtight")
				m_enumvector.push_back(E::eplusidtight);
			else if (string == "eplusidveto")
				m_enumvector.push_back(E::eplusidveto);
			else if (string == "eminusidloose")
				m_enumvector.push_back(E::eminusidloose);
			else if (string == "eminusidmedium")
				m_enumvector.push_back(E::eminusidmedium);
			else if (string == "eminusidtight")
				m_enumvector.push_back(E::eminusidtight);
			else if (string == "eminusidveto")
				m_enumvector.push_back(E::eminusidveto);
			else if (string == "algol5pt")
				m_enumvector.push_back(E::algol5pt);
			else if (string == "physl5pt")
				m_enumvector.push_back(E::physl5pt);
			else if (string == "mpfphys")
				m_enumvector.push_back(E::mpfphys);
			else if (string == "mpfalgo")
				m_enumvector.push_back(E::mpfalgo);
			else if (string == "mpfneutrinosalgo")
				m_enumvector.push_back(E::mpfneutrinosalgo);
			else if (string == "mpfneutrinosphys")
				m_enumvector.push_back(E::mpfneutrinosphys);
			else if (string == "jet1ptneutrinosalgo")
				m_enumvector.push_back(E::jet1ptneutrinosalgo);
			else if (string == "jet1ptneutrinosphys")
				m_enumvector.push_back(E::jet1ptneutrinosphys);
			else
				LOG_FATAL("NtupleConsumer: Quantity (" << string << ") not available!");

		}
	};

};


}
