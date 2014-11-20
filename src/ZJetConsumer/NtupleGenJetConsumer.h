#pragma once

#include <string>
#include <memory>

#include "GlobalInclude.h"
#include "RootTools/RootIncludes.h"
#include "NtupleConsumer.h"
#include "NtupleObjectConsumerBase.h"

/*
    This consumer creates an ntuple with an entry for each jet in the event.
*/


namespace Artus
{

class NtupleGenJetConsumer : public NtupleObjectConsumerBase
{
protected:

	virtual std::string GetObjectName() const
	{
		return "genjet";
	}

	virtual stringvector GetStringvector() const
	{
		stringvector s =
		{
			"recopt", "lhezpt", "hasmatch", "deltar", "deltarmuon", "flavour"
		};
		s.insert(s.end(), NtupleObjectConsumerBase::GetStringvector().begin(), NtupleObjectConsumerBase::GetStringvector().end());
		return s;
	}

	virtual int getsize(ZJetEventData const& event,
						ZJetProduct const& product, ZJetPipelineSettings const& s) const
	{
		return product.GetValidJetCount(s, event, JetType::GetGenName(s.GetJetAlgorithm()));
	}

	virtual KDataLV GetSingleObject(int n, ZJetEventData const& event,
									ZJetProduct const& product, ZJetPipelineSettings const& s) const
	{
		return * product.GetValidJet(s, event, n, JetType::GetGenName(s.GetJetAlgorithm()));
	};


	virtual float returnvalue(const int n, std::string string, ZJetEventData const& event,
							  ZJetProduct const& product, ZJetPipelineSettings const& s) const
	{
		if (string == "run")
			return event.m_eventproduct->nRun;
		else if (string == "recopt")
		{
			float deltar = 99.;
			float temp;
			float recopt = 0;
			for (const auto & recojet : product.m_validPFJets.at("AK5PFJetsCHSL1L2L3"))
			{
				temp = ROOT::Math::VectorUtil::DeltaR(recojet.p4,
													  GetSingleObject(n, event, product, s).p4);
				if (temp < deltar)
				{
					deltar = temp;
					recopt = recojet.p4.Pt();
					//return recopt;
				}
			}
			return recopt;
		}
		else if (string == "lhezpt")
			return product.GetLHEZ().p4.Pt();
		else if (string == "hasmatch")
		{
			for (const auto & recojet : product.m_validPFJets.at("AK5PFJetsCHSL1L2L3"))
			{
				if (ROOT::Math::VectorUtil::DeltaR(recojet.p4,
												   GetSingleObject(n, event, product, s).p4) < 0.25)
					return 1;
			}
			return 0.;
		}
		else if (string == "eventnr")
			return event.m_eventproduct->nEvent;
		else if (string == "deltar")
		{
			float deltar = 99.;
			float temp;
			for (const auto & recojet : product.m_validPFJets.at("AK5PFJetsCHSL1L2L3"))
			{
				temp = ROOT::Math::VectorUtil::DeltaR(recojet.p4,
													  GetSingleObject(n, event, product, s).p4);
				if (temp < deltar)
					deltar = temp;
			}
			return deltar;
		}
		else if (string == "deltarmuon")
		{
			float deltar = 99.;
			float temp;
			for (const auto & muon : product.GetValidMuons())
			{
				temp = ROOT::Math::VectorUtil::DeltaR(muon.p4,
													  GetSingleObject(n, event, product, s).p4);
				if (temp < deltar)
					deltar = temp;
			}
			return deltar;
		}
		else if (string == "flavour")
		{
			for (const auto parton : product.m_genPartons)
			{
				if (ROOT::Math::VectorUtil::DeltaR(parton.p4,
												   GetSingleObject(n, event, product, s).p4) < 0.3)
					return abs(parton.pdgId());

			}
			return 0.;
		}
		else
			return NtupleObjectConsumerBase::returnvalue(n, string, event, product, s);
	};

};


}
