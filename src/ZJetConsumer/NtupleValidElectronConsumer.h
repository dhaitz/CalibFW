#pragma once

#include <string>
#include <memory>

#include "GlobalInclude.h"
#include "RootTools/RootIncludes.h"
#include "NtupleConsumer.h"
#include "NtupleObjectConsumerBase.h"

/*
	This consumer creates an ntuple with an entry for each electron in the event.
*/


namespace Artus
{

class NtupleValidElectronConsumer : public NtupleObjectConsumerBase
{
protected:

	virtual std::string GetObjectName() const
	{
		return "electron";
	}

	virtual std::vector<std::string> GetStringvector() const
	{
		return {"pt", "eta", "phi", "mass",
				"mva", "mvaid", "mvatrigid", "looseid", "mediumid", "tightid",
				"deltar"
			   };
	}

	virtual int getsize(ZJetEventData const& event,
						ZJetProduct const& product, ZJetPipelineSettings const& settings) const
	{
		return product.m_listValidElectrons.size();
	}

	virtual KDataLV GetSingleObject(int n, ZJetEventData const& event,
									ZJetProduct const& product, ZJetPipelineSettings const& s) const
	{
		return product.m_listValidElectrons.at(n);
	};

	virtual float returnvalue(int n, std::string string, ZJetEventData const& event,
							  ZJetProduct const& product, ZJetPipelineSettings const& s) const
	{
		if (string == "mvaid")
			return product.m_listValidElectrons.at(n).idMvaNonTrigV0;
		else if (string == "mvatrigid")
			return product.m_listValidElectrons.at(n).idMvaTrigV0;
		else if (string == "mva")
		{
			const KDataElectron it = product.m_listValidElectrons.at(n);
			if (
				(
					(it.p4.Pt() < 10)
					&&
					(
						(abs(it.p4.Eta()) < 0.8 && it.idMvaNonTrigV0 > 0.47)
						|| (abs(it.p4.Eta()) > 0.8 && abs(it.p4.Eta()) < 1.479 && it.idMvaNonTrigV0 > 0.004)
						|| (abs(it.p4.Eta()) > 1.479 && abs(it.p4.Eta()) < 2.5 && it.idMvaNonTrigV0 > 0.295)
					)
				)
				||
				(
					(it.p4.Pt() > 10) &&
					(
						(abs(it.p4.Eta()) < 0.8 && it.idMvaNonTrigV0 > -0.34)
						|| (abs(it.p4.Eta()) > 0.8 && abs(it.p4.Eta()) < 1.479 && it.idMvaNonTrigV0 > -0.65)
						|| (abs(it.p4.Eta()) > 1.479 && abs(it.p4.Eta()) < 2.5 && it.idMvaNonTrigV0 > 0.6)
					)
				)
			)
				return 1.;
			else
				return 0.;
		}
		else if (string == "looseid")
			return product.m_listValidElectrons.at(n).cutbasedIDloose;
		else if (string == "mediumid")
			return product.m_listValidElectrons.at(n).cutbasedIDmedium;
		else if (string == "tightid")
			return product.m_listValidElectrons.at(n).cutbasedIDtight;
		else if (string == "deltar") // Delta R between matching reco and gen electron
		{
			for (auto it = product.m_genInternalElectrons.begin(); it != product.m_genInternalElectrons.end(); it++)
			{
				if (it->charge() == int(product.m_listValidElectrons.at(n).charge))
					return ROOT::Math::VectorUtil::DeltaR(GetSingleObject(n, event, product, s).p4, it->p4);
			}
			return 999;
		}
		else
			return NtupleObjectConsumerBase::returnvalue(n, string, event, product, s);
	};


};

}
