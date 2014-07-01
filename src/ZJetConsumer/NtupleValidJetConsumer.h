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
				"neutralhadfraction", "muonfraction", "HFhadfraction", "HFemfraction"
			   };
	}

	virtual int getsize(ZJetEventData const& event,
						ZJetMetaData const& metaData, ZJetPipelineSettings const& settings) const
	{
		return metaData.GetValidJetCount(settings, event);
	}

	virtual KDataLV GetSingleObject(int n, ZJetEventData const& event,
									ZJetMetaData const& metaData, ZJetPipelineSettings const& s) const
	{
		return * metaData.GetValidJet(s, event, n);
	};


	virtual float returnvalue(int n, std::string string, ZJetEventData const& event,
							  ZJetMetaData const& metaData, ZJetPipelineSettings const& s) const
	{
		if (string == "run")
			return event.m_eventmetadata->nRun;
		else if (string == "photonfraction")
			return static_cast<KDataPFJet*>(metaData.GetValidJet(s, event, n))->photonFraction;
		else if (string == "chargedemfraction")
			return static_cast<KDataPFJet*>(metaData.GetValidJet(s, event, n))->chargedEMFraction;
		else if (string == "chargedhadfraction")
			return static_cast<KDataPFJet*>(metaData.GetValidJet(s, event, n))->chargedHadFraction;
		else if (string == "neutralhadfraction")
			return static_cast<KDataPFJet*>(metaData.GetValidJet(s, event, n))->neutralHadFraction;
		else if (string == "muonfraction")
			return static_cast<KDataPFJet*>(metaData.GetValidJet(s, event, n))->muonFraction;
		else if (string == "HFhadfraction")
			return static_cast<KDataPFJet*>(metaData.GetValidJet(s, event, n))->HFHadFraction;
		else if (string == "HFemfraction")
			return static_cast<KDataPFJet*>(metaData.GetValidJet(s, event, n))->HFEMFraction;
		else
			return NtupleObjectConsumerBase::returnvalue(n, string, event, metaData, s);
	};

};


}
