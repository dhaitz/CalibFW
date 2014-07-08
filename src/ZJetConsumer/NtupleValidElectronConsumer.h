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
				"mvaid", "mvatrigid", "looseid", "mediumid", "tightid",
				"deltar"
			   };
	}

	virtual int getsize(ZJetEventData const& event,
						ZJetMetaData const& metaData, ZJetPipelineSettings const& settings) const
	{
		return metaData.m_listValidElectrons.size();
	}

	virtual KDataLV GetSingleObject(int n, ZJetEventData const& event,
									ZJetMetaData const& metaData, ZJetPipelineSettings const& s) const
	{
		return metaData.m_listValidElectrons.at(n);
	};

	virtual float returnvalue(int n, std::string string, ZJetEventData const& event,
							  ZJetMetaData const& metaData, ZJetPipelineSettings const& s) const
	{
		if (string == "mvaid")
			return metaData.m_listValidElectrons.at(n).idMvaNonTrigV0;
		else if (string == "mvatrigid")
			return metaData.m_listValidElectrons.at(n).idMvaTrigV0;
		else if (string == "looseid")
			return metaData.m_listValidElectrons.at(n).cutbasedIDloose;
		else if (string == "mediumid")
			return metaData.m_listValidElectrons.at(n).cutbasedIDmedium;
		else if (string == "tightid")
			return metaData.m_listValidElectrons.at(n).cutbasedIDtight;
		else if (string == "deltar") // Delta R between matching reco and gen electron
		{
			for (auto it = metaData.m_genInternalElectrons.begin(); it != metaData.m_genInternalElectrons.end(); it++)
			{
				if (it->charge() == int(metaData.m_listValidElectrons.at(n).charge))
					return ROOT::Math::VectorUtil::DeltaR(GetSingleObject(n, event, metaData, s).p4, it->p4);
			}
			return 999;
		}
		else
			return NtupleObjectConsumerBase::returnvalue(n, string, event, metaData, s);
	};


};

}
