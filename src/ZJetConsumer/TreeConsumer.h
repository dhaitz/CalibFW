#pragma once

#include <string>
#include <memory>

#include "GlobalInclude.h"
#include "RootTools/RootIncludes.h"


namespace CalibFW
{

template < class TEvent, class TMetaData, class TSettings >
class TreeConsumerBase : public EventConsumerBase< TEvent, TMetaData, TSettings>
{
public:
	typedef EventPipeline<TEvent, TMetaData, TSettings> PipelineTypeForThis;
};

class TreeConsumer : public TreeConsumerBase< ZJetEventData, ZJetMetaData, ZJetPipelineSettings >
{

	static std::string GetName()
	{
		return "tree_consumer";
	}

	void Init(PipelineTypeForThis* pset)
	{
		EventConsumerBase< ZJetEventData, ZJetMetaData, ZJetPipelineSettings>::Init(pset);
		std::string quantities = this->GetPipelineSettings().GetQuantitiesString();
		m_tree = new TNtuple("NTuple", "NTuple", quantities.c_str());
	}

	virtual void ProcessFilteredEvent(ZJetEventData const& event,
									  ZJetMetaData const& metaData)
	{
		EventConsumerBase< ZJetEventData, ZJetMetaData, ZJetPipelineSettings>::ProcessFilteredEvent(event, metaData);

		// get the list (stringvector) of variables
		std::vector<std::string> v = this->GetPipelineSettings().GetQuantities();

		std::vector<float> arr;

		// fill the array with values according to the variable-list :
		for (std::vector<std::string>::iterator it = v.begin(); it != v.end(); ++it)
			arr.push_back(returnvalue(*it, event, metaData, this->GetPipelineSettings()));

		// add the array to the tree
		m_tree->Fill(&arr[0]);
	}

	virtual void Finish()
	{
		ZJetPipelineSettings s = this->GetPipelineSettings();
		RootFileHelper::SafeCd(s.GetRootOutFile(), s.GetRootFileFolder());
		m_tree->Write((s.GetTreename() + "_" + s.GetJetAlgorithm()).c_str());
	}

private:

	std::string m_name;
	TNtuple* m_tree;

	enum type
	{
		npv, rho, run, weight, zpt, zeta, zphi, zy, zmass, jet1pt, jet1eta, jet1phi,
		jet1photonfraction, jet1chargedemfraction, jet1chargedhadfraction,
		jet1neutralhadfraction, jet1muonfraction, jet1HFhadfraction,
		jet1HFemfraction, jet2pt, jet2phi, jet2eta, METpt, METphi, sumEt,
		rawMETpt, rawMETphi, uept, uephi, ueeta, mpf, rawmpf, otherjetspt,
		otherjetsphi, otherjetseta, genjet1pt, genjet1eta, genjet1phi,
		matchedgenjet1pt, genjet2pt, genzpt, genmpf, algoflavour, physflavour,
		mupluspt, mupluseta, muplusphi, muminuspt, muminuseta, muminusphi,
		pionpt, kshortpt, neutralpt, neutralptnocut, algopartonpt,
		subleadingjetspt, subleadingjetsphi,
		qglikelihood, qgmlp, trackcountinghigheffbjettag,
		trackcountinghighpurbjettag, jetprobabilitybjettag,
		jetbprobabilitybjettag, softelectronbjettag, softmuonbjettag,
		softmuonbyip3dbjettag, softmuonbyptbjettag, simplesecondaryvertexbjettag,
		combinedsecondaryvertexbjettag,  combinedsecondaryvertexmvabjettag,
		jet1puJetFull, jet1puJetIDFull, jet1puJetIDFullLoose, jet1puJetIDFullMedium, jet1puJetIDFullTight,
		jet1puJetCutbased, jet1puJetIDCutbased, jet1puJetIDCutbasedLoose, jet1puJetIDCutbasedMedium, jet1puJetIDCutbasedTight,
		jet2puJetFull, jet2puJetIDFull, jet2puJetIDFullLoose, jet2puJetIDFullMedium, jet2puJetIDFullTight,
		jet2puJetCutbased, jet2puJetIDCutbased, jet2puJetIDCutbasedLoose, jet2puJetIDCutbasedMedium, jet2puJetIDCutbasedTight
	} var;

	float returnvalue(std::string string, ZJetEventData const& event,
					  ZJetMetaData const& metaData, ZJetPipelineSettings const& s)
	{

		int n;
		if (string == "npv") var = npv;
		else if (string == "rho") var = rho;
		else if (string == "run") var = run;
		else if (string == "weight") var = weight;

		else if (string == "zpt") var = zpt;
		else if (string == "zeta") var = zeta;
		else if (string == "zphi") var = zphi;
		else if (string == "zmass") var = zmass;
		else if (string == "zy") var = zy;

		else if (string == "jet1pt") var = jet1pt;
		else if (string == "jet1eta") var = jet1eta;
		else if (string == "jet1phi") var = jet1phi;
		else if (string == "jet1photonfraction") var = jet1photonfraction;
		else if (string == "jet1chargedemfraction") var = jet1chargedemfraction;
		else if (string == "jet1chargedhadfraction") var = jet1chargedhadfraction;
		else if (string == "jet1neutralhadfraction") var = jet1neutralhadfraction;
		else if (string == "jet1muonfraction") var = jet1muonfraction;
		else if (string == "jet1HFhadfraction") var = jet1HFhadfraction;
		else if (string == "jet1HFemfraction") var = jet1HFemfraction;
		else if (string == "jet2pt") var = jet2pt;
		else if (string == "jet2phi") var = jet2phi;
		else if (string == "jet2eta") var = jet2eta;
		else if (string == "METpt") var = METpt;
		else if (string == "METphi") var = METphi;
		else if (string == "sumEt") var = sumEt;
		else if (string == "rawMETpt") var = rawMETpt;
		else if (string == "rawMETphi") var = rawMETphi;
		else if (string == "uept") var = uept;
		else if (string == "uephi") var = uephi;
		else if (string == "ueeta") var = ueeta;
		else if (string == "mpf") var = mpf;
		else if (string == "rawmpf") var = rawmpf;
		else if (string == "otherjetspt") var = otherjetspt;
		else if (string == "otherjetsphi") var = otherjetsphi;
		else if (string == "otherjetseta") var = otherjetseta;
		else if (string == "genjet1pt") var = genjet1pt;
		else if (string == "genjet1eta") var = genjet1eta;
		else if (string == "genjet1phi") var = genjet1phi;
		else if (string == "matchedgenjet1pt") var = matchedgenjet1pt;
		else if (string == "genjet2pt") var = genjet2pt;
		else if (string == "genzpt") var = genzpt;
		else if (string == "genmpf") var = genmpf;
		else if (string == "algoflavour") var = algoflavour;
		else if (string == "physflavour") var = physflavour;

		else if (string == "mupluspt") var = mupluspt;
		else if (string == "mupluseta") var = mupluseta;
		else if (string == "muplusphi") var = muplusphi;

		else if (string == "muminuspt") var = muminuspt;
		else if (string == "muminuseta") var = muminuseta;
		else if (string == "muminusphi") var = muminusphi;

		else if (string == "qglikelihood") var = qglikelihood;
		else if (string == "qgmlp") var = qgmlp;

		else if (string == "trackcountinghigheffbjettag") var = trackcountinghigheffbjettag;
		else if (string == "trackcountinghighpurbjettag") var = trackcountinghighpurbjettag;
		else if (string == "jetprobabilitybjettag") var = jetprobabilitybjettag;
		else if (string == "jetbprobabilitybjettag") var = jetbprobabilitybjettag;
		else if (string == "softelectronbjettag") var = softelectronbjettag;
		else if (string == "softmuonbjettag") var = softmuonbjettag;
		else if (string == "softmuonbyip3dbjettag") var = softmuonbyip3dbjettag;
		else if (string == "softmuonbyptbjettag") var = softmuonbyptbjettag;
		else if (string == "simplesecondaryvertexbjettag") var = simplesecondaryvertexbjettag;
		else if (string == "combinedsecondaryvertexbjettag") var = combinedsecondaryvertexbjettag;
		else if (string == "combinedsecondaryvertexmvabjettag") var = combinedsecondaryvertexmvabjettag;

		else if (string == "jet1puJetFull") var = jet1puJetFull;
		else if (string == "jet1puJetIDFull") var = jet1puJetIDFull;
		else if (string == "jet1puJetIDFullLoose") var = jet1puJetIDFullLoose;
		else if (string == "jet1puJetIDFullMedium") var = jet1puJetIDFullMedium;
		else if (string == "jet1puJetIDFullTight") var = jet1puJetIDFullTight;
		else if (string == "jet1puJetCutbased") var = jet1puJetCutbased;
		else if (string == "jet1puJetIDCutbased") var = jet1puJetIDCutbased;
		else if (string == "jet1puJetIDCutbasedLoose") var = jet1puJetIDCutbasedLoose;
		else if (string == "jet1puJetIDCutbasedMedium") var = jet1puJetIDCutbasedMedium;
		else if (string == "jet1puJetIDCutbasedTight") var = jet1puJetIDCutbasedTight;

		else if (string == "jet2puJetFull") var = jet2puJetFull;
		else if (string == "jet2puJetIDFull") var = jet2puJetIDFull;
		else if (string == "jet2puJetIDFullLoose") var = jet2puJetIDFullLoose;
		else if (string == "jet2puJetIDFullMedium") var = jet2puJetIDFullMedium;
		else if (string == "jet2puJetIDFullTight") var = jet2puJetIDFullTight;
		else if (string == "jet2puJetCutbased") var = jet2puJetCutbased;
		else if (string == "jet2puJetIDCutbased") var = jet2puJetIDCutbased;
		else if (string == "jet2puJetIDCutbasedLoose") var = jet2puJetIDCutbasedLoose;
		else if (string == "jet2puJetIDCutbasedMedium") var = jet2puJetIDCutbasedMedium;
		else if (string == "jet2puJetIDCutbasedTight") var = jet2puJetIDCutbasedTight;



		// general quantities
		if (var == npv)
			return event.m_vertexSummary->nVertices;

		// QG tag
		else if (var == qglikelihood)
		{
			if (static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->qgLikelihood == -1)
				return -999;
			else
				return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->qgLikelihood;
		}
		else if (var == qgmlp)
		{
			if (static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->qgLikelihood == -1)
				return -999;
			else
				return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->qgLikelihood;
		}

		// b tags
		else if (var == trackcountinghigheffbjettag)
		{
			if (static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->trackCountingHighEffBTag == -1)
				return -999;
			else
				return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->trackCountingHighEffBTag;
		}
		else if (var == trackcountinghighpurbjettag)
		{
			if (static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->trackCountingHighPurBTag == -1)
				return -999;
			else
				return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->trackCountingHighPurBTag;
		}
		else if (var == jetprobabilitybjettag)
		{
			if (static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->jetProbabilityBTag == -1)
				return -999;
			else
				return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->jetProbabilityBTag;
		}
		else if (var == jetbprobabilitybjettag)
		{
			if (static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->jetBProbabilityBTag == -1)
				return -999;
			else
				return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->jetBProbabilityBTag;
		}
		else if (var == softelectronbjettag)
		{
			if (static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->softElectronBTag == -1)
				return -999;
			else
				return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->softElectronBTag;
		}
		else if (var == softmuonbjettag)
		{
			if (static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->softMuonBTag == -1)
				return -999;
			else
				return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->softMuonBTag;
		}
		else if (var == softmuonbyip3dbjettag)
		{
			if (static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->softMuonByIP3dBTag == -1)
				return -999;
			else
				return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->softMuonByIP3dBTag;
		}
		else if (var == softmuonbyptbjettag)
		{
			if (static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->softMuonByPtBTag == -1)
				return -999;
			else
				return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->softMuonByPtBTag;
		}
		else if (var == simplesecondaryvertexbjettag)
		{
			if (static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->simpleSecondaryVertexBTag == -1)
				return -999;
			else
				return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->simpleSecondaryVertexBTag;
		}
		else if (var == combinedsecondaryvertexbjettag)
		{
			if (static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->combinedSecondaryVertexBTag == -1)
				return -999;
			else
				return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->combinedSecondaryVertexBTag;
		}
		else if (var == combinedsecondaryvertexmvabjettag)
		{
			if (static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->combinedSecondaryVertexMVABTag == -1)
				return -999;
			else
				return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->combinedSecondaryVertexMVABTag;
		}

		// jet 1 PU
		else if (var == jet1puJetFull)
			return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->puJetFull;
		else if (var == jet1puJetIDFull)
			return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->puJetIDFull;
		else if (var == jet1puJetIDFullLoose)
			return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->puJetIDFullLoose;
		else if (var == jet1puJetIDFullMedium)
			return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->puJetIDFullMedium;
		else if (var == jet1puJetIDFullTight)
			return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->puJetIDFullTight;

		else if (var == jet1puJetCutbased)
			return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->puJetCutbased;
		else if (var == jet1puJetIDCutbased)
			return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->puJetIDCutbased;
		else if (var == jet1puJetIDCutbasedLoose)
			return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->puJetIDCutbasedLoose;
		else if (var == jet1puJetIDCutbasedMedium)
			return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->puJetIDCutbasedMedium;
		else if (var == jet1puJetIDCutbasedTight)
			return static_cast<KDataPFTaggedJet*>(metaData.GetValidPrimaryJet(s, event))->puJetIDCutbasedTight;


		// jet 2 PU
		else if (var == jet2puJetFull)
		{
			if (metaData.GetValidJetCount(s, event) > 1)
				return static_cast<KDataPFTaggedJet*>(metaData.GetValidJet(s, event, 1))->puJetFull;
			else return 0;
		}
		else if (var == jet2puJetIDFull)
		{
			if (metaData.GetValidJetCount(s, event) > 1)
				return static_cast<KDataPFTaggedJet*>(metaData.GetValidJet(s, event, 1))->puJetIDFull;
			else return 0;
		}
		else if (var == jet2puJetIDFullLoose)
		{
			if (metaData.GetValidJetCount(s, event) > 1)
				return static_cast<KDataPFTaggedJet*>(metaData.GetValidJet(s, event, 1))->puJetIDFullLoose;
			else return 0;
		}
		else if (var == jet2puJetIDFullMedium)
		{
			if (metaData.GetValidJetCount(s, event) > 1)
				return static_cast<KDataPFTaggedJet*>(metaData.GetValidJet(s, event, 1))->puJetIDFullMedium;
			else return 0;
		}
		else if (var == jet2puJetIDFullTight)
		{
			if (metaData.GetValidJetCount(s, event) > 1)
				return static_cast<KDataPFTaggedJet*>(metaData.GetValidJet(s, event, 1))->puJetIDFullTight;
			else return 0;
		}
		else if (var == jet2puJetCutbased)
		{
			if (metaData.GetValidJetCount(s, event) > 1)
				return static_cast<KDataPFTaggedJet*>(metaData.GetValidJet(s, event, 1))->puJetCutbased;
			else return 0;
		}
		else if (var == jet2puJetIDCutbased)
		{
			if (metaData.GetValidJetCount(s, event) > 1)
				return static_cast<KDataPFTaggedJet*>(metaData.GetValidJet(s, event, 1))->puJetIDCutbased;
			else return 0;
		}
		else if (var == jet2puJetIDCutbasedLoose)
		{
			if (metaData.GetValidJetCount(s, event) > 1)
				return static_cast<KDataPFTaggedJet*>(metaData.GetValidJet(s, event, 1))->puJetIDCutbasedLoose;
			else return 0;
		}
		else if (var == jet2puJetIDCutbasedMedium)
		{
			if (metaData.GetValidJetCount(s, event) > 1)
				return static_cast<KDataPFTaggedJet*>(metaData.GetValidJet(s, event, 1))->puJetIDCutbasedMedium;
			else return 0;
		}
		else if (var == jet2puJetIDCutbasedTight)
		{
			if (metaData.GetValidJetCount(s, event) > 1)
				return static_cast<KDataPFTaggedJet*>(metaData.GetValidJet(s, event, 1))->puJetIDCutbasedTight;
			else return 0;
		}



		else if (var == rho)
			return event.m_jetArea->median;
		else if (var == run)
			return event.m_eventmetadata->nRun;
		else if (var == weight)
			return metaData.GetWeight();
		// Z
		else if (var == zpt)
			return metaData.GetRefZ().p4.Pt();
		else if (var == zeta)
			return metaData.GetRefZ().p4.Eta();
		else if (var == zphi)
			return metaData.GetRefZ().p4.Phi();
		else if (var == zy)
			return metaData.GetRefZ().p4.Rapidity();
		else if (var == zmass)
			return metaData.GetRefZ().p4.mass();

		// muons
		else if (var == mupluspt)
		{
			for (KDataMuons::const_iterator it = metaData.m_listValidMuons.begin();
				 it != metaData.m_listValidMuons.end(); it ++)
			{
				if (it->charge == 1) return it->p4.Pt();
			}
		}
		else if (var == mupluseta)
		{
			for (KDataMuons::const_iterator it = metaData.m_listValidMuons.begin();
				 it != metaData.m_listValidMuons.end(); it ++)
			{
				if (it->charge == 1) return it->p4.Eta();
			}
		}
		else if (var == muplusphi)
		{
			for (KDataMuons::const_iterator it = metaData.m_listValidMuons.begin();
				 it != metaData.m_listValidMuons.end(); it ++)
			{
				if (it->charge == 1) return it->p4.Phi();
			}
		}
		else if (var == muminuspt)
		{
			for (KDataMuons::const_iterator it = metaData.m_listValidMuons.begin();
				 it != metaData.m_listValidMuons.end(); it ++)
			{
				if (it->charge == -1) return it->p4.Pt();
			}
		}
		else if (var == muminuseta)
		{
			for (KDataMuons::const_iterator it = metaData.m_listValidMuons.begin();
				 it != metaData.m_listValidMuons.end(); it ++)
			{
				if (it->charge == -1) return it->p4.Eta();
			}
		}
		else if (var == muminusphi)
		{
			for (KDataMuons::const_iterator it = metaData.m_listValidMuons.begin();
				 it != metaData.m_listValidMuons.end(); it ++)
			{
				if (it->charge == -1) return it->p4.Phi();
			}
		}

		// leading jet
		else if (var == jet1pt)
			return metaData.GetValidPrimaryJet(s, event)->p4.Pt();
		else if (var == jet1eta)
			return metaData.GetValidPrimaryJet(s, event)->p4.Eta();
		else if (var == jet1phi)
			return metaData.GetValidPrimaryJet(s, event)->p4.Phi();

		// leading jet composition
		else if (var == jet1photonfraction)
			return static_cast<KDataPFJet*>(metaData.GetValidPrimaryJet(s, event))->photonFraction;
		else if (var == jet1chargedemfraction)
			return static_cast<KDataPFJet*>(metaData.GetValidPrimaryJet(s, event))->chargedEMFraction;
		else if (var == jet1chargedhadfraction)
			return static_cast<KDataPFJet*>(metaData.GetValidPrimaryJet(s, event))->chargedHadFraction;
		else if (var == jet1neutralhadfraction)
			return static_cast<KDataPFJet*>(metaData.GetValidPrimaryJet(s, event))->neutralHadFraction;
		else if (var == jet1muonfraction)
			return static_cast<KDataPFJet*>(metaData.GetValidPrimaryJet(s, event))->muonFraction;
		else if (var == jet1HFhadfraction)
			return static_cast<KDataPFJet*>(metaData.GetValidPrimaryJet(s, event))->HFHadFraction;
		else if (var == jet1HFemfraction)
			return static_cast<KDataPFJet*>(metaData.GetValidPrimaryJet(s, event))->HFEMFraction;

		// second jet
		else if (var == jet2pt)
		{
			if (metaData.GetValidJetCount(s, event) > 1)
				return metaData.GetValidJet(s, event, 1)->p4.Pt();
			else
				return 0;
		}
		else if (var == jet2phi)
		{
			if (metaData.GetValidJetCount(s, event) > 1)
				return metaData.GetValidJet(s, event, 1)->p4.Phi();
			else
				return 0;
		}
		else if (var == jet2eta)
		{
			if (metaData.GetValidJetCount(s, event) > 1)
				return metaData.GetValidJet(s, event, 1)->p4.Eta();
			else
				return 0;
		}

		// MET & sumEt
		else if (var == METpt)
			return metaData.GetMet(event, s)->p4.Pt();
		else if (var == METphi)
			return metaData.GetMet(event, s)->p4.Phi();
		else if (var == sumEt)
			return metaData.GetMet(event, s)->sumEt;
		else if (var == rawMETpt)
			return event.GetMet(s)->p4.Pt();
		else if (var == rawMETphi)
			return event.GetMet(s)->p4.Phi();

		else if (var == uept)
			return metaData.GetUE(event, s)->p4.Pt();
		else if (var == uephi)
			return metaData.GetUE(event, s)->p4.Phi();
		else if (var == ueeta)
			return metaData.GetUE(event, s)->p4.Eta();
		else if (var == mpf)
			return metaData.GetMPF(metaData.GetMet(event, s));
		else if (var == rawmpf)
			return metaData.GetMPF(event.GetMet(s));
		else if (var == otherjetspt)
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
		else if (var == otherjetsphi)
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
		else if (var == otherjetseta)
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
		else if (var == genjet1pt)
		{
			std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));

			if (metaData.GetValidJetCount(s, event, genName) == 0)
				return false;

			return metaData.GetValidJet(s, event, 0, genName)->p4.Pt();
		}
		else if (var == genjet1eta)
		{
			std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));

			if (metaData.GetValidJetCount(s, event, genName) == 0)
				return false;

			return metaData.GetValidJet(s, event, 0, genName)->p4.Eta();
		}
		else if (var == genjet1phi)
		{
			std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));

			if (metaData.GetValidJetCount(s, event, genName) == 0)
				return false;

			return metaData.GetValidJet(s, event, 0, genName)->p4.Phi();
		}
		else if (var == matchedgenjet1pt)
		{
			std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));

			KDataLV* matched_genjet = NULL;

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

			if (iMatchedGen >= metaData.GetValidJetCount(s, event, genName))
			{
				CALIB_LOG_FATAL("Reco to gen matching: No reference gen jet found! "
								<< iMatchedGen << " >= " << metaData.GetValidJetCount(s, event, genName))
				return false;
			}
			return metaData.GetValidJet(s, event, iMatchedGen, genName)->p4.Pt();
		}

		else if (var == genjet2pt)
		{
			std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));

			if (metaData.GetValidJetCount(s, event, genName) < 2)
				return false;

			return metaData.GetValidJet(s, event, 1, genName)->p4.Pt();
		}
		else if (var == genzpt)
			return metaData.GetRefGenZ().p4.Pt();
		else if (var == genmpf)
			return metaData.GetGenMPF(metaData.GetPtGenMet()), metaData.GetWeight();
		else if (var == algoflavour)
			return metaData.GetAlgoFlavour(s);
		else if (var == physflavour)
			return metaData.GetPhysFlavour(s);
		else
			CALIB_LOG_FATAL("TTreeConsumer: Quantity " << n << " not available!")
		};

    };

}
