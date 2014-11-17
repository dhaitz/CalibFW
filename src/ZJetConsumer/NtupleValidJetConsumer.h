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

class NtupleValidJetConsumer : public NtupleObjectConsumerBase
{
protected:

	virtual std::string GetObjectName() const
	{
		return "jet";
	}

	virtual std::vector<std::string> GetStringvector() const
	{
		return {"pt", "eta", "phi", "run",
				"photonfraction", "chargedemfraction", "chargedhadfraction",
				"neutralhadfraction", "muonfraction", "HFhadfraction", "HFemfraction",
				"genpt", "rawpt", "l1pt", "lhezpt", "hasmatch", "eventnr"
			   };
	}

	virtual int getsize(ZJetEventData const& event,
						ZJetProduct const& product, ZJetPipelineSettings const& settings) const
	{
		return product.GetValidJetCount(settings, event);
	}

	virtual KDataLV GetSingleObject(int n, ZJetEventData const& event,
									ZJetProduct const& product, ZJetPipelineSettings const& s) const
	{
		return * product.GetValidJet(s, event, n);
	};


	virtual float returnvalue(const int n, std::string string, ZJetEventData const& event,
							  ZJetProduct const& product, ZJetPipelineSettings const& s) const
	{
		if (string == "run")
			return event.m_eventproduct->nRun;
		else if (string == "photonfraction")
			return static_cast<KDataPFJet*>(product.GetValidJet(s, event, n))->photonFraction;
		else if (string == "chargedemfraction")
			return static_cast<KDataPFJet*>(product.GetValidJet(s, event, n))->chargedEMFraction;
		else if (string == "chargedhadfraction")
			return static_cast<KDataPFJet*>(product.GetValidJet(s, event, n))->chargedHadFraction;
		else if (string == "neutralhadfraction")
			return static_cast<KDataPFJet*>(product.GetValidJet(s, event, n))->neutralHadFraction;
		else if (string == "muonfraction")
			return static_cast<KDataPFJet*>(product.GetValidJet(s, event, n))->muonFraction;
		else if (string == "HFhadfraction")
			return static_cast<KDataPFJet*>(product.GetValidJet(s, event, n))->HFHadFraction;
		else if (string == "HFemfraction")
			return static_cast<KDataPFJet*>(product.GetValidJet(s, event, n))->HFEMFraction;
		else if (string == "genpt")
		{
			std::string genName(JetType::GetGenName(s.GetJetAlgorithm()));

			if (product.m_matchingResults.at(this->GetPipelineSettings().GetJetAlgorithm()).at(n) > -1)
			{
				if (product.GetLHEZ().p4.Pt() > 24.44 && product.GetLHEZ().p4.Pt() < 24.46)
					product.m_matchingResults.at(this->GetPipelineSettings().GetJetAlgorithm()).at(n),
												 genName)->p4.Pt())
					return product.GetValidJet(s, event,
											   product.m_matchingResults.at(this->GetPipelineSettings().GetJetAlgorithm()).at(n),
											   genName)->p4.Pt();

				}
			else
				return 0;

		}
		else if (string == "rawpt")
		{
			for (auto rawjet : *event.m_pfTaggedJets.at("AK5PFTaggedJetsCHS"))
			{
				if (ROOT::Math::VectorUtil::DeltaR(rawjet.p4,
												   GetSingleObject(n, event, product, s).p4) < 0.01)
					return rawjet.p4.Pt();
			}
			return 0.;
		}
		else if (string == "l1pt")
			return product.m_validPFJets.at("AK5PFJetsCHSL1").at(n).p4.Pt();
		else if (string == "lhezpt")
			return product.GetLHEZ().p4.Pt();
		else if (string == "hasmatch")
			return float(product.m_matchingResults.at(this->GetPipelineSettings().GetJetAlgorithm()).at(n) > -1);
		else if (string == "eventnr")
			return event.m_eventproduct->nEvent;

		else
			return NtupleObjectConsumerBase::returnvalue(n, string, event, product, s);
	};

};


}
